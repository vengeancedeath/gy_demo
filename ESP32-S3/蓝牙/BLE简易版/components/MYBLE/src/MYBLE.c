#include <stdio.h>
#include "MYBLE.h"
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#define TAG "MYBLE"
#define DEVICE_NAME "ESP32-MYBLE"

static bool ble_adv_active = false;
static uint16_t rx_value_handler;   //接收句柄
static uint16_t tx_value_handler;   //发送句柄

uint8_t tx_cnt = 0;



//连接回调
static int gap_event_handler(struct ble_gap_event *event, void *arg)
{

    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:  //发生蓝牙连接事件
        /* code */
        if(event->connect.status == 0)
        {
            ble_adv_active = false;
            ESP_LOGI(TAG,"设备已连接");//连接成功 广播自动关闭
        }
        else
        {
            ESP_LOGI(TAG,"设备连接失败");
            if(ble_adv_active == false)
            {
                start_advertising();
            }
        }


        break;
    case BLE_GAP_EVENT_DISCONNECT: //发生蓝牙断开事件 不会自动开启广播
        /* code */
        ESP_LOGI(TAG,"设备断开连接");
        if(ble_adv_active == false)
        {
            start_advertising();
        }
        break;
    
    default:
        break;
    }
    return 0;
}

//数据通信回调
static int gatt_event_handler(uint16_t conn_handle, uint16_t attr_handle,
                               struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    switch (ctxt->op)
    {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        /* code */
        if (attr_handle == tx_value_handler)
        {
            /* code */
            const char * tx_data = "HELLO";
            os_mbuf_append(ctxt->om, tx_data, strlen(tx_data));
        }
        
        break;
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        /* code */
        if(attr_handle == rx_value_handler)
        {
            if(ctxt->om->om_data[0] == 0x00)
            {
                tx_cnt++;
                uint8_t data = ctxt->om->om_data[0];
                ESP_LOGI(TAG,"receive data : 0x%02d,cnt :0x%02d",data,tx_cnt);
            }
            else
            {
                tx_cnt--;
                uint8_t data = ctxt->om->om_data[0];
                ESP_LOGI(TAG,"receive data : 0x%02d,cnt :0x%02d",data,tx_cnt);
            }
        }
        break;
    
    default:
        break;
    }

    return 0;

}


static const struct ble_gatt_svc_def gatt_svcs[] = {
    { 
        .type = BLE_GATT_SVC_TYPE_PRIMARY,  //主要服务
        .uuid = BLE_UUID16_DECLARE(0x00FF),  //UUID
        // .includes = ,  //次级服务
        .characteristics = (struct ble_gatt_chr_def[])
        {
            //ESP32S3接收数据
            {
                .uuid = BLE_UUID16_DECLARE(0xFF01),
                .access_cb = gatt_event_handler,  //GATT事件回调
                .flags = BLE_GATT_CHR_F_WRITE,   //描述符
                .val_handle = &rx_value_handler, //接收句柄
                .arg = NULL,
            },
            //ESP32S3发送数据
            {
                .uuid = BLE_UUID16_DECLARE(0xFF02),
                .access_cb = gatt_event_handler,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &tx_value_handler,  //发送句柄
                .arg = NULL,
            },
            {0}

        }
        


    },
    {0}
};


void start_advertising(void)
{
    int res = 0;

    struct ble_hs_adv_fields fields = {0};
    fields.name = (uint8_t *)DEVICE_NAME;  //设备名称
    fields.name_len = strlen(DEVICE_NAME); ////设备名称长度
    fields.name_is_complete = 1;    //设备名称是否是缩写
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;  //通用可发现模式 | 仅支持低功耗蓝牙 不支持经典蓝牙
    fields.tx_pwr_lvl_is_present = 1; //广播是否携带发送功率信息
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;  //自动

    res = ble_gap_adv_set_fields(&fields);  //设置广播参数
    if(res != 0)
    {
        ESP_LOGI(TAG,"广播信息设置失败");
        return;
    }

    struct ble_gap_adv_params adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,  //非定向链接
        .disc_mode = BLE_GAP_DISC_MODE_GEN, //一直可发现
        .itvl_max = BLE_GAP_ADV_ITVL_MS(200),
        .itvl_min = BLE_GAP_ADV_ITVL_MS(200),
        
    };

    res = ble_gap_adv_start(     //开启广播
                BLE_OWN_ADDR_PUBLIC,  //公共地址
                NULL, //非定向广播 
                BLE_HS_FOREVER,  //无限时间广播
                &adv_params, //广播内容
                gap_event_handler,   //GAP事件回调
                NULL);
    if(res != 0)
    {
        ESP_LOGI(TAG,"广播开启失败");
        return;
    }
    else
    {
        ESP_LOGI(TAG,"广播开启成功，等待连接.......");
        ble_adv_active = true;
        return;
    }
    // ble_gap_adv_stop();
}

static void on_sync(void)
{
    ESP_LOGI(TAG,"设备初始化结束");
    start_advertising(); //设置开始广播
}

void host_task( void * arg)
{
    nimble_port_run();

}

void ble_init(void)
{
    ESP_LOGI(TAG, "ble_init");
    nimble_port_init(); //初始化蓝牙协议栈

    ble_svc_gap_init(); //初始化GAP连接标准
    ble_svc_gatt_init();//初始化GATT通信标准

    ble_svc_gap_device_name_set(DEVICE_NAME); //设置设备名称

    ble_gatts_count_cfg(gatt_svcs); 
    ble_gatts_add_svcs(gatt_svcs); //注册服务特征列表
    
    ble_hs_cfg.sync_cb = on_sync;  //初始化完毕回调

    nimble_port_freertos_init(host_task);  //创建监听任务
}
