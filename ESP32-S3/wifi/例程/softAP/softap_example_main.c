/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

/* The examples use WiFi configuration that you can set via project configuration menu.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
//menuconfig配置的wifi ssid
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID         
//menuconfig配置的wifi password
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD    
//menuconfig配置的wifi channel信道- 中国 1-13常用2.4G信道互相重叠，1，6，11互不重叠 推荐6常用 
//AP模式不能写0  APSTA双模时信道会被STA侧强制同步
#define EXAMPLE_ESP_WIFI_CHANNEL   CONFIG_ESP_WIFI_CHANNEL     
//menuconfig配置的max sta conn num 最大连接数为4 
#define EXAMPLE_MAX_STA_CONN       CONFIG_ESP_MAX_STA_CONN      

//是否开启 GTK 自动重密钥功能（AP 模式专用）
#if CONFIG_ESP_GTK_REKEYING_ENABLE  
/* 重密钥周期，单位：秒；多久轮换一次 GTK 密钥。*/
#define EXAMPLE_GTK_REKEY_INTERVAL CONFIG_ESP_GTK_REKEY_INTERVAL  
#else
#define EXAMPLE_GTK_REKEY_INTERVAL 0
#endif

static const char *TAG = "wifi softAP";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        //处理STA连接事件 打印设备的 MAC 地址和 AID
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        //处理STA断开事件 打印设备的 MAC 地址和 AID 及断开原因
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

void wifi_init_softap(void)
{
    //2.初始化LWIP协议栈
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "initialize lwip");
    //3.创建默认循环事件组
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "create default event loop");
    //4.将STA/AP模块和LWIP协议栈连接
    esp_netif_create_default_wifi_ap();
    ESP_LOGI(TAG, "create default wifi ap");
    //6.初始化WIFI模块
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "initialize wifi");
    //7.注册事件处理函数
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));
    ESP_LOGI(TAG, "register event handler instance");
    //8.配置WIFI参数
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,  // 热点名
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),  // SSID 长度
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,  // 信道
            .password = EXAMPLE_ESP_WIFI_PASS,    // 密码
            .max_connection = EXAMPLE_MAX_STA_CONN,   // 最大连接数(4)
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT  //是否启用了WPA3支持
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true, //  WPA3 要求 PMF(受保护管理帧) 必须开启。
            },
#ifdef CONFIG_ESP_WIFI_BSS_MAX_IDLE_SUPPORT  //AP 检测接入的 STA 长时间空闲无流量，主动踢掉这个设备
            .bss_max_idle_cfg = {    
                .period = WIFI_AP_DEFAULT_MAX_IDLE_PERIOD,  // 最大空闲时间周期 秒
                .protected_keep_alive = 1,  //使用 PMF 保护的保活报文
            },
#endif
            .gtk_rekey_interval = EXAMPLE_GTK_REKEY_INTERVAL, // GTK 重密钥间隔 秒
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {  //密码是空字符串，强制切换成开放热点 WIFI_AUTH_OPEN
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    //9.设置为AP模式
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_LOGI(TAG, "set wifi mode to ap");
    //10.设置AP参数
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_LOGI(TAG, "set wifi config");
    //11.启动WIFI模块
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "start wifi");

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

void app_main(void)
{
    //1.Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "initialize nvs flash");

    wifi_init_softap();
}
