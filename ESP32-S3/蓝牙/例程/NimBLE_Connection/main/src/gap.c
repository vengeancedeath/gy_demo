/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Includes */
#include "gap.h"
#include "common.h"

/* Private function declarations */
inline static void format_addr(char *addr_str, uint8_t addr[]);
static void print_conn_desc(struct ble_gap_conn_desc *desc);
static void start_advertising(void);
static int gap_event_handler(struct ble_gap_event *event, void *arg);

/* Private variables */
static uint8_t own_addr_type; // 蓝牙地址类型
static uint8_t addr_val[6] = {0};  //MAC地址
static uint8_t esp_uri[] = {BLE_GAP_URI_PREFIX_HTTPS, '/', '/', 'e', 's', 'p', 'r', 'e', 's', 's', 'i', 'f', '.', 'c', 'o', 'm'};
//广播携带的url数据

/* Private functions */  //格式化蓝牙地址
inline static void format_addr(char *addr_str, uint8_t addr[]) {
    sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1],
            addr[2], addr[3], addr[4], addr[5]);
}

//打印连接描述信息
static void print_conn_desc(struct ble_gap_conn_desc *desc) {
    /* Local variables */
    char addr_str[18] = {0};

    /* Connection handle */
    ESP_LOGI(TAG, "connection handle: %d", desc->conn_handle);  // 连接句柄

    /* Local ID address */
    format_addr(addr_str, desc->our_id_addr.val);  // 格式化本地ID地址
    ESP_LOGI(TAG, "device id address: type=%d, value=%s",
             desc->our_id_addr.type, addr_str);

    /* Peer ID address */
    format_addr(addr_str, desc->peer_id_addr.val);  // 格式化对端ID地址
    ESP_LOGI(TAG, "peer id address: type=%d, value=%s", desc->peer_id_addr.type,
             addr_str);

    /* Connection info */
    // 连接信息 间隔、延迟、监督时长、是否加密、是否认证、是否绑定对端
    ESP_LOGI(TAG,
             "conn_itvl=%d, conn_latency=%d, supervision_timeout=%d, "
             "encrypted=%d, authenticated=%d, bonded=%d\n",
             desc->conn_itvl, desc->conn_latency, desc->supervision_timeout,
             desc->sec_state.encrypted, desc->sec_state.authenticated,
             desc->sec_state.bonded);
}

static void start_advertising(void) {
    /* Local variables */
    int rc = 0;
    const char *name;
    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields rsp_fields = {0};
    struct ble_gap_adv_params adv_params = {0};

    /* Set advertising flags */
    // 广播标志位 一般可发现模式 | 不支持 BR/EDR
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Set device name */
    name = ble_svc_gap_device_name();  // 获取设备名称
    adv_fields.name = (uint8_t *)name; // 设备名称
    adv_fields.name_len = strlen(name); // 设备名称长度
    adv_fields.name_is_complete = 1; // 设备名称是否完整

    /* Set device tx power */
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;  // 自动设置功率
    adv_fields.tx_pwr_lvl_is_present = 1;  // 标记功率字段为有效

    /* Set device appearance */
    adv_fields.appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;  // 设外观为通用标签
    adv_fields.appearance_is_present = 1;  // 标记外观字段为有效

    /* Set device LE role */
    //**外设 (Peripheral)，只可以被别人连接，不能主动发起连接**。
    adv_fields.le_role = BLE_GAP_LE_ROLE_PERIPHERAL;  // 设为外设角色 
    adv_fields.le_role_is_present = 1;  // 标记角色字段为有效

    /* Set advertisement fields */
    rc = ble_gap_adv_set_fields(&adv_fields); // 设置广播数据
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set advertising data, error code: %d", rc);
        return;
    }

    /* Set device address */
    rsp_fields.device_addr = addr_val;  // 设备地址
    rsp_fields.device_addr_type = own_addr_type;  // 设备地址类型
    rsp_fields.device_addr_is_present = 1;  // 标记地址字段为有效

    /* Set URI */
    rsp_fields.uri = esp_uri;  // 设备URI
    rsp_fields.uri_len = sizeof(esp_uri);  // 设备URI长度

    /* Set advertising interval */
    rsp_fields.adv_itvl = BLE_GAP_ADV_ITVL_MS(500);  // 广播间隔 500ms
    rsp_fields.adv_itvl_is_present = 1;

    /* Set scan response fields */ //当中心设备发送"主动扫描"请求时，设备会返回这些额外信息：
    rc = ble_gap_adv_rsp_set_fields(&rsp_fields); // 设置扫描响应数据
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set scan response data, error code: %d", rc);
        return;
    }

    /* Set non-connectable and general discoverable mode to be a beacon */
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;  //  非定向可连接广播
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;  // 一般可发现模式

    /* Set advertising interval */
    adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(500);  
    adv_params.itvl_max = BLE_GAP_ADV_ITVL_MS(510);

    /* Start advertising */
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                           gap_event_handler, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}

/*
 * NimBLE applies an event-driven model to keep GAP service going
 * gap_event_handler is a callback function registered when calling
 * ble_gap_adv_start API and called when a GAP event arrives
 */
static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    /* Local variables */
    int rc = 0;
    struct ble_gap_conn_desc desc;

    /* Handle different GAP event */
    switch (event->type) {

    /* Connect event */
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        ESP_LOGI(TAG, "connection %s; status=%d",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);

        /* Connection succeeded */
        if (event->connect.status == 0) {
            /* Check connection handle  */ //根据连接句柄查找连接的描述信息
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc != 0) {
                ESP_LOGE(TAG,
                         "failed to find connection by handle, error code: %d",
                         rc);
                return rc;
            }

            /* Print connection descriptor and turn on the LED */
            print_conn_desc(&desc); // 打印连接参数

            /* Try to update connection parameters */
            struct ble_gap_upd_params params = {.itvl_min = desc.conn_itvl,  // 保持当前连接间隔
                                                .itvl_max = desc.conn_itvl,  // 保持当前连接间隔
                                                //外设可以跳过最多 3 个连接事件不响应，从而节省功耗。比如连接间隔是 30ms，延迟为 3，则外设最长可以在 120ms 内不响应中心设备
                                                .latency = 3,   // 从设备延迟设为 3
                                                .supervision_timeout = //监督超时时间，用于检测连接是否仍然存活  supervision_timeout > (1 + latency) × itvl_max × 2  (单位统一后)
                                                    desc.supervision_timeout};
            rc = ble_gap_update_params(event->connect.conn_handle, &params);  //更新连接参数
            if (rc != 0) {
                ESP_LOGE(
                    TAG,
                    "failed to update connection parameters, error code: %d",
                    rc);
                return rc;
            }
        }
        /* Connection failed, restart advertising */
        else {
            start_advertising();
        }
        return rc;

    /* Disconnect event */
    case BLE_GAP_EVENT_DISCONNECT:
        /* A connection was terminated, print connection descriptor */
        ESP_LOGI(TAG, "disconnected from peer; reason=%d",
                 event->disconnect.reason);


        /* Restart advertising */
        start_advertising();
        return rc;

    /* Connection parameters update event */
    case BLE_GAP_EVENT_CONN_UPDATE:  //连接参数更新事件
        /* The central has updated the connection parameters. */
        ESP_LOGI(TAG, "connection updated; status=%d",
                 event->conn_update.status);
        // status == 0 表示更新成功
        // status != 0 表示对端拒绝了更新请求

        /* Print connection descriptor */
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGE(TAG, "failed to find connection by handle, error code: %d",
                     rc);
            return rc;
        }
        print_conn_desc(&desc);  // 打印连接参数
        return rc;
    }

    return rc;
}

/* Public functions */
void adv_init(void) {
    /* Local variables */
    int rc = 0;
    char addr_str[18] = {0};

    /* Make sure we have proper BT identity address set */
    rc = ble_hs_util_ensure_addr(0);  // 确保设备有可用的蓝牙地址
    if (rc != 0) {
        ESP_LOGE(TAG, "device does not have any available bt address!");
        return;
    }

    /* Figure out BT address to use while advertising */
    rc = ble_hs_id_infer_auto(0, &own_addr_type); //自动推断地址类型
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to infer address type, error code: %d", rc);
        return;
    }

    /* Copy device address to addr_val */
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL); // 复制蓝牙地址到addr_val
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to copy device address, error code: %d", rc);
        return;
    }
    format_addr(addr_str, addr_val); // 格式化蓝牙地址为字符串
    ESP_LOGI(TAG, "device address: %s", addr_str);

    /* Start advertising. */
    start_advertising(); // 广播
}

// 初始化 GAP 服务
int gap_init(void) {  
    /* Local variables */
    int rc = 0;

    /* Initialize GAP service */
    ble_svc_gap_init();  // 初始化GAP连接标志

    /* Set GAP device name */
    rc = ble_svc_gap_device_name_set(DEVICE_NAME);  // 设置设备名称
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set device name to %s, error code: %d",
                 DEVICE_NAME, rc);
        return rc;
    }

    /* Set GAP device appearance */
    rc = ble_svc_gap_device_appearance_set(BLE_GAP_APPEARANCE_GENERIC_TAG);  //设置 GAP Appearance（外观特征值）
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set device appearance, error code: %d", rc);
        return rc;
    }
    return rc;
}
