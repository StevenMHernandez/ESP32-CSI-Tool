#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include <esp_wifi_internal.h>
#include "esp_event_loop.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "nvs_component.h"
#include "sd_component.h"
#include "csi_component.h"
#include "time_component.h"
#include "input_component.h"
#include "sockets_component.h"

/*
 * The examples use WiFi configuration that you can set via 'make menuconfig'.
 *
 * If you'd rather not, just change the below entries to strings with
 * the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
 */
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define EXAMPLE_ESP_MAXIMUM_RETRY  10

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "Active CSI collection (Station)";

static int s_retry_num = 0;

esp_err_t _http_event_handle(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                if (!real_time_set) {
                    char *data = malloc(evt->data_len + 1);
                    strncpy(data, evt->data, evt->data_len);
                    data[evt->data_len + 1] = '\0';
                    time_set(data);
                    free(data);
                }
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}

esp_http_client_config_t config = {
        .url = "https://192.168.4.1:80",
        .event_handler = _http_event_handle,
        .is_async = true, // this option allows for much higher packet rates, but requires https.
};

//// en_sys_seq: see https://github.com/espressif/esp-idf/blob/master/docs/api-guides/wifi.rst#wi-fi-80211-packet-send for details
esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "got ip:%s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            s_retry_num = 0;
            xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED: {
            if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
                esp_wifi_connect();
                xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
                s_retry_num++;
                ESP_LOGI(TAG, "retry to connect to the AP");
            }
            ESP_LOGI(TAG, "connect to the AP fail\n");
            break;
        }
        default:
            break;
    }
    return ESP_OK;
}

bool is_wifi_connected() {
    return (xEventGroupGetBits(s_wifi_event_group) & WIFI_CONNECTED_BIT);
}

void station_init() {
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_config_t wifi_config = {
            .sta = {
                    .ssid = EXAMPLE_ESP_WIFI_SSID,
                    .password = EXAMPLE_ESP_WIFI_PASS,
            },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_set_ps(WIFI_PS_NONE);

    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

void app_main() {
    nvs_init();
    sd_init();
    station_init();
    csi_init("STA");

//    socket_listener_loop();
    socket_transmitter_sta_loop(&is_wifi_connected);
}
