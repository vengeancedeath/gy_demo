/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Includes */
#include "gatt_svc.h"
#include "common.h"
#include "heart_rate.h"

/* Private function declarations */
static int heart_rate_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt, void *arg);
static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg);

/* Private variables */
/* Heart rate service */
static const ble_uuid16_t heart_rate_svc_uuid = BLE_UUID16_INIT(0x180D);


// [0] = Flags 字: 0 表示后续心率值是 UINT8 格式
// [1] = 心率值: 60~80 的随机数
static uint8_t heart_rate_chr_val[2] = {0};
static uint16_t heart_rate_chr_val_handle;   // 特征值value句柄
static const ble_uuid16_t heart_rate_chr_uuid = BLE_UUID16_INIT(0x2A37);

static uint16_t heart_rate_chr_conn_handle = BLE_HS_CONN_HANDLE_NONE;  // 连接句柄（后续Indication需要）
static bool heart_rate_chr_conn_handle_inited = false;  // 连接句柄是否设置过
static bool heart_rate_ind_status = false;  //是否启用Indication（带确认的通知）（默认不启用Indication）

/* Automation IO service */
static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);
static uint16_t led_chr_val_handle;   // 特征值value句柄
static const ble_uuid128_t led_chr_uuid = 
    BLE_UUID128_INIT(0x23, 0xd1, 0xbc, 0xea, 0x5f, 0x78, 0x23, 0x15, 0xde, 0xef,
                     0x12, 0x12, 0x25, 0x15, 0x00, 0x00);
//显示的UUID和实际的UUID相反，因为蓝牙 SIG 标准定义的 UUID 是小端序，而 NimBLE 蓝牙协议栈是大端序

/* GATT services table */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    /* Heart rate service */
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,  // 主服务
        //通过偏移量读取后面的 value (uint16_t)
     .uuid = &heart_rate_svc_uuid.u,  // 心率服务UUID  蓝牙 SIG 标准定义的 UUID 
     .characteristics =
         (struct ble_gatt_chr_def[]){
             {/* Heart rate characteristic */
              .uuid = &heart_rate_chr_uuid.u,   // UUID = 0x2A37（心率测量）  蓝牙 SIG 标准定义的 UUID
              .access_cb = heart_rate_chr_access,  // 读写回调函数
              .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE,  // 可读  可Indicate（带确认的通知）
              .val_handle = &heart_rate_chr_val_handle},  // 保存特征值句柄（后续Indication需要）
             {
                 0/* No more characteristics in this service. */  // 结束标记
             }
        }
    },

    /* Automation IO service */
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,  // 主服务
        .uuid = &auto_io_svc_uuid.u,   // UUID = 0x1815（自动化IO服务）
        .characteristics =
            (struct ble_gatt_chr_def[]){/* LED characteristic */
                                        {.uuid = &led_chr_uuid.u, // 128-bit 自定义UUID
                                         .access_cb = led_chr_access,   // 写回调函数
                                         .flags = BLE_GATT_CHR_F_WRITE,   // 只写
                                         .val_handle = &led_chr_val_handle},
                                        {0}},  // 结束标记
    },

    {
        0 /* No more services. */  // 结束标记
    },
};

/* Private functions */
static int heart_rate_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt, void *arg) {
    /* Local variables */
    int rc = 0;

    /* Handle access events */
    /* Note: Heart rate characteristic is read only */
    switch (ctxt->op) {

    /* Read characteristic event */
    case BLE_GATT_ACCESS_OP_READ_CHR:   // 手机发来读请求
        /* Verify connection handle */
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            ESP_LOGI(TAG, "characteristic read; conn_handle=%d attr_handle=%d",
                     conn_handle, attr_handle);
        } else {  //来自协议内部的读请求 比如Indication（带确认的通知）请求
            ESP_LOGI(TAG, "characteristic read by nimble stack; attr_handle=%d",
                     attr_handle);
        }

        /* Verify attribute handle */
        if (attr_handle == heart_rate_chr_val_handle) {
            /* Update access buffer value */
            heart_rate_chr_val[1] = get_heart_rate();   // 获取最新心率值
            // 将数据追加到响应缓冲区
            rc = os_mbuf_append(ctxt->om, &heart_rate_chr_val,
                                sizeof(heart_rate_chr_val));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;  // 资源不足，无法完成请求
        }
        goto error;

    /* Unknown event */
    default:
        goto error;  // 心率特征值只支持读，其他操作返回错误
    }

error:
    ESP_LOGE(
        TAG,
        "unexpected access operation to heart rate characteristic, opcode: %d",
        ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
}

static int led_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                          struct ble_gatt_access_ctxt *ctxt, void *arg) {
    /* Local variables */
    int rc = 0;

    /* Handle access events */
    /* Note: LED characteristic is write only */
    switch (ctxt->op) {

    /* Write characteristic event */
    case BLE_GATT_ACCESS_OP_WRITE_CHR:  // 手机发来写请求
        /* Verify connection handle */
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            ESP_LOGI(TAG, "characteristic write; conn_handle=%d attr_handle=%d",
                     conn_handle, attr_handle);
        } else {
            ESP_LOGI(TAG,
                     "characteristic write by nimble stack; attr_handle=%d",
                     attr_handle);
        }

        /* Verify attribute handle */
        if (attr_handle == led_chr_val_handle) {
            /* Verify access buffer length */
            if (ctxt->om->om_len == 1) {  // 只接受1字节数据
                /* Turn the LED on or off according to the operation bit */
                if (ctxt->om->om_data[0]) {
                    ESP_LOGI(TAG, "led turned on!");
                } else {
                    ESP_LOGI(TAG, "led turned off!");
                }
            } else {
                goto error;
            }
            return rc;
        }
        goto error;

    /* Unknown event */
    default:
        goto error; // LED特征值只支持写，读操作返回错误
    }

error:
    ESP_LOGE(TAG,
             "unexpected access operation to led characteristic, opcode: %d",
             ctxt->op);
    return BLE_ATT_ERR_UNLIKELY;
}

/* Public functions */
void send_heart_rate_indication(void) {
    // 只有手机订阅了 且 连接句柄已初始化，才发送
    if (heart_rate_ind_status && heart_rate_chr_conn_handle_inited) {
        ble_gatts_indicate(heart_rate_chr_conn_handle,  //发送Indication通知
                           heart_rate_chr_val_handle);
        ESP_LOGI(TAG, "heart rate indication sent!");
    }
}

/*    
 *  Handle GATT attribute register events
 *      - Service register event
 *      - Characteristic register event
 *      - Descriptor register event
 */

//不执行业务逻辑，仅用于调试日志打印
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    /* Local variables */
    char buf[BLE_UUID_STR_LEN];

    /* Handle GATT attributes register events */
    switch (ctxt->op) {

    /* Service register event */
    case BLE_GATT_REGISTER_OP_SVC:  // 服务注册完成
    // 打印: 服务的 UUID + 分配的 handle

        ESP_LOGD(TAG, "registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),   // UUID 转字符串
                 ctxt->svc.handle);   // 服务 handle
        break;

    /* Characteristic register event */
    case BLE_GATT_REGISTER_OP_CHR:  // 特征值注册完成
    // 打印: 特征值的 UUID + 定义 handle + 值 handle
        ESP_LOGD(TAG,
                 "registering characteristic %s with "
                 "def_handle=%d val_handle=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),   // UUID 转字符串
                 ctxt->chr.def_handle,   // 特征值Declaration属性的 handle
                 ctxt->chr.val_handle);    // 特征Value数据属性的 handle（读写操作用这个）
        break;

    /* Descriptor register event */
    case BLE_GATT_REGISTER_OP_DSC: // 描述符注册完成
    // 打印: 描述符的 UUID + 分配的 handle
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),   // UUID 转字符串
                 ctxt->dsc.handle);   // 描述符 handle
        break;

    /* Unknown event */
    default:
        assert(0);
        break;
    }
}

/*
 *  GATT server subscribe event callback
 *      1. Update heart rate subscription status
 */
//订阅回调 — 管理心率 Indication 开关
void gatt_svr_subscribe_cb(struct ble_gap_event *event) {
    /* Check connection handle */
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE) { //代表无有效连接句柄
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle);
    } else {
        ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                 event->subscribe.attr_handle);
    }

    /* Check attribute handle */
    if (event->subscribe.attr_handle == heart_rate_chr_val_handle) {
        /* Update heart rate subscription status */
        // 记录：哪个连接订阅了、当前是否订阅了Indication
        heart_rate_chr_conn_handle = event->subscribe.conn_handle;  // 连接句柄
        heart_rate_chr_conn_handle_inited = true;  // 连接句柄已初始化
        heart_rate_ind_status = event->subscribe.cur_indicate;   // true=订阅, false=取消
    }
}

void gatt_svr_reset_heart_rate_subscription(void) {
    heart_rate_chr_conn_handle = BLE_HS_CONN_HANDLE_NONE;
    heart_rate_chr_conn_handle_inited = false;
    heart_rate_ind_status = false;
}

/*
 *  GATT server initialization
 *      1. Initialize GATT service
 *      2. Update NimBLE host GATT services counter
 *      3. Add GATT services to server
 */
int gatt_svc_init(void) {
    /* Local variables */
    int rc = 0;

    /* 1. GATT service initialization */
    ble_svc_gatt_init(); // 初始化 GATT 通信标志

    /* 2. Update GATT services counter */
    rc = ble_gatts_count_cfg(gatt_svr_svcs);  //统计服务表中的服务数量
    if (rc != 0) {
        return rc;
    }

    /* 3. Add GATT services */
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to add GATT services, error code: %d", rc);
        return rc;
    }

    return 0;
}