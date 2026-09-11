#include "ble_gap.h"
#include "services/gap/ble_svc_gap.h"
#include "ble_gatt.h"
#include "esp_random.h"
#include "host/util/util.h"

static const char * TAG = "BLE_GAP";

inline static void format_addr(char *addr_str, uint8_t addr[]);
static void print_conn_desc(struct ble_gap_conn_desc *desc);
static void start_advertising(void);
static void set_random_addr(void);
static int gap_event_handler(struct ble_gap_event *event, void *arg);

static uint8_t own_addr_type; // 蓝牙地址类型
static uint8_t addr_val[6] = {0};  //MAC地址
static uint8_t esp_uri[] = {BLE_GAP_URI_PREFIX_HTTPS, '/', '/', 'e', 's', 'p', 'r', 'e', 's', 's', 'i', 'f', '.', 'c', 'o', 'm'};
//广播携带的url数据

//格式化蓝牙地址
inline static void format_addr(char *addr_str, uint8_t addr[]) {
    sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1],
            addr[2], addr[3], addr[4], addr[5]);
}


// 设置随机地址
static void set_random_addr(void) {
    /* Local variables */
    int rc = 0;
    ble_addr_t addr;

   //生成一个不可解析的随机私有地址（Non-Resolvable Private Address, NRPA）
   //0 表示不指定地址类型，由协议栈自动选择 
   rc = ble_hs_id_gen_rnd(0, &addr);
    assert(rc == 0); //不可恢复错误，生成随机地址失败，程序会崩溃

   // 写入 NimBLE 协议栈，作为本设备的随机地址
    rc = ble_hs_id_set_rnd(addr.val);
    assert(rc == 0); //不可恢复错误，设置随机地址失败，程序会崩溃
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
    else{
        ESP_LOGI(TAG, "广播数据设置成功");
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
    else{
        ESP_LOGI(TAG, "扫描响应数据设置成功");
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


static int gap_event_handler(struct ble_gap_event *event, void *arg) {
    /* Local variables */
    int rc = 0;
    struct ble_gap_conn_desc desc;

    /* Handle different GAP event */
    switch (event->type) {

    /* Connect event */
    case BLE_GAP_EVENT_CONNECT:  //发生连接时触发
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
    case BLE_GAP_EVENT_DISCONNECT:  // 连接断开事件
        /* A connection was terminated, print connection descriptor */
        ESP_LOGI(TAG, "disconnected from peer; reason=%d",
                 event->disconnect.reason);

        //gatt_svr_reset_heart_rate_subscription();   // 新增！重置订阅状态

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

    /* Advertising complete event */
    /*
        reason=0：因为建立连接，广播自动停止（最常用）
        reason=1：因为超时，广播自动停止
        reason=BLE_HS_EPREEMPTED：软件主动调用 ble_gap_adv_stop () 手动停止广播
    */
    case BLE_GAP_EVENT_ADV_COMPLETE:  
        /* Advertising completed, restart advertising */
        ESP_LOGI(TAG, "advertise complete; reason=%d",
                 event->adv_complete.reason);
        start_advertising();
        return rc;

    /* Notification sent event */
    case BLE_GAP_EVENT_NOTIFY_TX:  
    // notify indication 共用 发送完成事件
    //event->notify_tx.indication == 1 表示是 Indication 送完成事件
    //event->notify_tx.indication == 0 表示是 普通 Notify（无 ACK，status 只是代表数据包空中发送完成，不是客户端应答）
    
    // - status == 0 → Notify 发送正常交给控制器
    // - status == 14 → Indication 收到客户端 ACK
    
        if ((event->notify_tx.status != 0) &&       
            (event->notify_tx.status != BLE_HS_EDONE)) {
                // 发送失败时打印日志
            /* Print notification info on error */
            ESP_LOGI(TAG,
                     "notify event; conn_handle=%d attr_handle=%d "
                     "status=%d is_indication=%d",
                     event->notify_tx.conn_handle, event->notify_tx.attr_handle,
                     event->notify_tx.status, event->notify_tx.indication);
        }
        
        if ((event->notify_tx.status != 0) &&       
            (event->notify_tx.status == BLE_HS_EDONE)) {
                // 发送失败时打印日志
            /* Print notification info on error */
            ESP_LOGI(TAG,
                     "notify indication ack event; conn_handle=%d attr_handle=%d "
                     "status=%d is_indication=%d",
                     event->notify_tx.conn_handle, event->notify_tx.attr_handle,
                     event->notify_tx.status, event->notify_tx.indication);
        }
        return rc;

    /* Subscribe event */
    /*
        订阅事件  客户端修改 CCCD，Notify/Indicate 开关发生变化时触发
        注：连接时会触发1801的2A05事件 不确定都会还是软件触发
    */
    case BLE_GAP_EVENT_SUBSCRIBE:  
        /* Print subscription info to log */
        ESP_LOGI(TAG,
                 "subscribe event; conn_handle=%d attr_handle=%d "
                 "reason=%d prevn=%d curn=%d previ=%d curi=%d",
                 //conn_handle`：当前 BLE 连接的句柄
                 //attr_handle`：发生变更的**Characteristic 特征的属性句柄**
                 event->subscribe.conn_handle, event->subscribe.attr_handle,  // 连接句柄、特征属性句柄
                 event->subscribe.reason, event->subscribe.prev_notify,  // 订阅事件原因、上一时刻Notify使能状态
                 event->subscribe.cur_notify, event->subscribe.prev_indicate,  // 当前Notify使能状态、上一时刻Indicate使能状态
                 event->subscribe.cur_indicate);  // 当前Indicate使能状态

         // 委托给gatt_svc处理
        rc = gatt_svr_subscribe_cb(event); 
        if (rc == BLE_ATT_ERR_INSUFFICIENT_AUTHEN) {
            /* Request connection encryption */
            return ble_gap_security_initiate(event->subscribe.conn_handle);  //主动发起配对请求
        }
        return rc;

    /* MTU update event */
    // 连接后自动协商 MTU，影响单次传输最大数据量
    case BLE_GAP_EVENT_MTU:  // MTU 协商完成事件  MTU决定了单次传输的最大数据量。默认 23 字节，协商后可以更大（如 512），提升吞吐量。
        /* Print MTU update info to log */
        ESP_LOGI(TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.channel_id,
                 event->mtu.value);
        return rc;

    /* Encryption change event */
    case BLE_GAP_EVENT_ENC_CHANGE:  //加密状态变化 → 配对成功/失败日志
        /* Encryption has been enabled or disabled for this connection. */
        if (event->enc_change.status == 0) {
            ESP_LOGI(TAG, "connection encrypted!");
        } else {
            ESP_LOGE(TAG, "connection encryption failed, status: %d",
                     event->enc_change.status);
        }
        return rc;

    /* Repeat pairing event */
    case BLE_GAP_EVENT_REPEAT_PAIRING:  //重复配对事件 - 已存在绑定记录但配对失败，怎么处理？
        /* Delete the old bond */
        //查找连接描述信息
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGE(TAG, "failed to find connection, error code %d", rc);
            return rc;
        }
         //删除旧的绑定记录（用对端蓝牙地址查找并删除NVS中保存的密钥）
        ble_store_util_delete_peer(&desc.peer_id_addr);  

        /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
         * continue with pairing operation */
        ESP_LOGI(TAG, "repairing...");
        return BLE_GAP_REPEAT_PAIRING_RETRY;

    /* Passkey action event */
    case BLE_GAP_EVENT_PASSKEY_ACTION:  //配对码事件 → 生成6位随机配对码 → 显示在串口日志上 → 用户在手机端输入
        /* Display action */
        //判断协议栈要求的是显示（Display）动作——即本设备需要显示配对码给用户看
        if (event->passkey.params.action == BLE_SM_IOACT_DISP) {
            /* Generate passkey */
            struct ble_sm_io pkey = {0};
            pkey.action = event->passkey.params.action;
            pkey.passkey = 100000 + esp_random() % 900000;  // 生成 100000~999999 的6位码
            ESP_LOGI(TAG, "enter passkey %" PRIu32 " on the peer side",
                     pkey.passkey);
            rc = ble_sm_inject_io(event->passkey.conn_handle, &pkey);  // 注入配对码到安全管理器
            if (rc != 0) {
                ESP_LOGE(TAG,
                         "failed to inject security manager io, error code: %d",
                         rc);
                return rc;
            }
        }
        return rc;
    }
    return rc;
}

// 初始化广告
void adv_init(void) {
    /* Local variables */
    int rc = 0;
    char addr_str[18] = {0};

    //rc = ble_hs_util_ensure_addr(0);  // 确保设备有可用的蓝牙地址

    //0 :优先使用public address
    //1 :优先使用random address
	    set_random_addr();  // 生成随机私有地址
    rc = ble_hs_util_ensure_addr(1);  // 确保设备有可用的随机地址
    if (rc != 0) {
        ESP_LOGE(TAG, "device does not have any available bt address!");
        return;
    }

    //自动推断地址类型
    rc = ble_hs_id_infer_auto(0, &own_addr_type); 
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to infer address type, error code: %d", rc);
        return;
    }

    // 复制蓝牙地址到addr_val
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL); 
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to copy device address, error code: %d", rc);
        return;
    }
    // 格式化蓝牙地址为字符串
    format_addr(addr_str, addr_val); 
    ESP_LOGI(TAG, "device address: %s", addr_str);

    // 开始广播
    start_advertising(); 
}

// 初始化 GAP 服务
int gap_init(void) {  
    /* Local variables */
    int rc = 0;

    // 初始化GAP服务
    ble_svc_gap_init();  

    // 设置设备名称
    rc = ble_svc_gap_device_name_set(DEVICE_NAME);  
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set device name to %s, error code: %d",
                 DEVICE_NAME, rc);
        return rc;
    }

    /* Set GAP device appearance */
    /*rc = ble_svc_gap_device_appearance_set(BLE_GAP_APPEARANCE_GENERIC_TAG);  //设置 GAP Appearance（外观特征值）
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set device appearance, error code: %d", rc);
        return rc;
    } */
    return rc;
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


// 检查连接是否已加密
bool is_connection_encrypted(uint16_t conn_handle) {
    /* Local variables */
    int rc = 0;
    struct ble_gap_conn_desc desc;

    /* Print connection descriptor */
    rc = ble_gap_conn_find(conn_handle, &desc);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to find connection by handle, error code: %d",
                 rc);
        return false;
    }

    return desc.sec_state.encrypted;
}