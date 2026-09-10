/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Includes */
#include "common.h"
#include "gap.h"
#include "gatt_svc.h"
#include "heart_rate.h"


/* Library function declarations */
void ble_store_config_init(void);

/* Private function declarations */
static void on_stack_reset(int reason);
static void on_stack_sync(void);
static void nimble_host_config_init(void);
static void nimble_host_task(void *param);

/* Private functions */
/*
 *  Stack event callback functions
 *      - on_stack_reset is called when host resets BLE stack due to errors
 *      - on_stack_sync is called when host has synced with controller
 */
// 协议栈重置回调
static void on_stack_reset(int reason) {
    /* On reset, print reset reason to console */
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}
// 协议栈同步完毕回调
static void on_stack_sync(void) {
    /* On stack sync, do advertising initialization */
    adv_init();
}

static void nimble_host_config_init(void) {
    /* Set host callbacks */
    ble_hs_cfg.reset_cb = on_stack_reset; // 注册协议栈重置回调
    ble_hs_cfg.sync_cb = on_stack_sync; // 注册协议栈同步完毕回调
	ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;  //GATT 注册回调  当 NimBLE 内部注册服务/特征值/描述符时，会调用这个回调，打印日志方便调试
    /*
        配对数据存储状态回调，与 BLE 安全配对（Bonding）机制密切相关。
        当 NimBLE 协议栈执行**配对/绑定（Pairing/Bonding）**操作时，\
        需要把密钥等数据持久化存储到 NVS Flash 中。
        存储操作可能成功，也可能失败（比如 NVS 空间不足）。\
        无论成功还是失败，协议栈都会调用这个回调来通知应用层。

        一句话理解：告诉 NimBLE 协议栈——
        "如果配对数据存不下了（NVS 满了），就自动删掉最旧的配对记录，然后重试存储。"
    */ 
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Security manager configuration */
    ble_hs_cfg.sm_io_cap = BLE_HS_IO_DISPLAY_ONLY;  // IO能力：仅显示（本设备有屏幕显示配对码，无键盘输入）
    ble_hs_cfg.sm_bonding = 1;   // 启用绑定（配对后保存密钥，下次免配对）
    // ble_hs_cfg.sm_sc = 1;         // 开启安全连接 Secure Connections（BLE4.2+加密算法，推荐打开）
    ble_hs_cfg.sm_mitm = 1;    // 启用 MITM 保护（中间人攻击防护，需要配对码） - 强制要求 MITM 保护，这意味着必须使用 Passkey 配对（不能 Just Works）
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;  // // 我方(ESP)要发给手机：LTK加密密钥 + IRK身份密钥
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID;  // 期望手机 (主机) 发送给我方 (ESP32)：LTK加密密钥 + IRK身份密钥

    /* Store host configuration */
    ble_store_config_init();   //初始化存储配对数据
}

static void nimble_host_task(void *param) {
    /* Task entry log */
    ESP_LOGI(TAG, "nimble host task has been started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();  // 运行 NimBLE 监听任务

    /* Clean up at exit */
    vTaskDelete(NULL);
}

static void heart_rate_task(void *param) {
    /* Task entry log */
    ESP_LOGI(TAG, "heart rate task has been started!");

    /* Loop forever */
    while (1) {
        /* Update heart rate value every 1 second */
        update_heart_rate();// 更新心率值（随机60~80）
        ESP_LOGI(TAG, "heart rate updated to %d", get_heart_rate());

        /* Send heart rate indication if enabled */
        send_heart_rate_indication(); // 如果手机订阅了，就发送Indication

        /* Sleep */
        vTaskDelay(HEART_RATE_TASK_PERIOD); // 等待1秒
    }

    /* Clean up at exit */
    vTaskDelete(NULL);
}

void app_main(void) {
    /* Local variables */
    BaseType_t rc = 0;
    esp_err_t ret = ESP_OK;


    uint32_t seed = esp_random();  // 生成随机种子
	srand(seed);  // 初始化随机数种子

    //CONFIG_LOG_MAXIMUM_LEVEL_DEBUG=y
    esp_log_level_set("NimBLE_Security", ESP_LOG_DEBUG); // 设置日志级别为 DEBUG，方便调试

    /* NVS flash initialization */
    ret = nvs_flash_init(); // 初始化 NVS flash
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", ret);
        return;
    }

    /* NimBLE host stack initialization */
    ret = nimble_port_init();  // 初始化 NimBLE 蓝牙协议栈
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nimble stack, error code: %d ",
                 ret);
        return;
    }

    ESP_LOGI(TAG,"初始化......");
    ESP_LOGD(TAG,"...........LOGD");

// #if CONFIG_BT_NIMBLE_GAP_SERVICE
    /* GAP service initialization */
    rc = gap_init();  // 初始化 GAP 服务(设备名称)
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to initialize GAP service, error code: %d", rc);
        return;
    }
// #endif

    /* GATT server initialization */
    rc = gatt_svc_init();  // 初始化 GATT 服务(注册服务表)
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to initialize GATT server, error code: %d", rc);
        return;
    }

    /* NimBLE host configuration initialization */
    nimble_host_config_init();    //配置HOST回调

    /* Start NimBLE host task thread and return */
    rc = xTaskCreate(nimble_host_task, "NimBLE Host", 4 * 1024, NULL,
                                5, NULL);
    if (rc != pdPASS) {
        ESP_LOGE(TAG, "failed to create NimBLE host task");
        return;
    }

    // 创建心率任务
    rc = xTaskCreate(heart_rate_task, "Heart Rate", 4 * 1024, NULL, 5, NULL);
    if (rc != pdPASS) {
        ESP_LOGE(TAG, "failed to create heart rate task");
        return;
    }
    return;
}
