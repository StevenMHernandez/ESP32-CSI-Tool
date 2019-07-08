#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

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
    return ESP_OK;
}

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

void print_csv_header() {
    printf("type,role,mac,rssi,rate,sig_mode,mcs,bandwidth,smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,secondary_channel,timestamp,ant,sig_len,rx_state,len,CSI_DATA\n");
}

void wifi_csi_cb(void *ctx, wifi_csi_info_t *data) {
    wifi_csi_info_t d = data[0];

    char mac[20] = {0};
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", d.mac[0], d.mac[1], d.mac[2], d.mac[3], d.mac[4], d.mac[5]);

    printf("%s,", "CSI_DATA");
    printf("%s,", "STA");
    printf("%s,", mac);

    // https://github.com/espressif/esp-idf/blob/9d0ca60398481a44861542638cfdc1949bb6f312/components/esp_wifi/include/esp_wifi_types.h#L314
    printf("%d,", d.rx_ctrl.rssi);
    printf("%d,", d.rx_ctrl.rate);
    printf("%d,", d.rx_ctrl.sig_mode);
    printf("%d,", d.rx_ctrl.mcs);
    printf("%d,", d.rx_ctrl.cwb);
    printf("%d,", d.rx_ctrl.smoothing);
    printf("%d,", d.rx_ctrl.not_sounding);
    printf("%d,", d.rx_ctrl.aggregation);
    printf("%d,", d.rx_ctrl.stbc);
    printf("%d,", d.rx_ctrl.fec_coding);
    printf("%d,", d.rx_ctrl.sgi);
    printf("%d,", d.rx_ctrl.noise_floor);
    printf("%d,", d.rx_ctrl.ampdu_cnt);
    printf("%d,", d.rx_ctrl.channel);
    printf("%d,", d.rx_ctrl.secondary_channel);
    printf("%d,", d.rx_ctrl.timestamp);
    printf("%d,", d.rx_ctrl.ant);
    printf("%d,", d.rx_ctrl.sig_len);
    printf("%d,", d.rx_ctrl.rx_state);

    printf("%d,", data->len);
    int8_t *my_ptr = data->buf;
    for (int i = 0; i < 128; i++) {
        printf("%d ", my_ptr[i]);
    }
    printf("\n");
}

void csi_init() {
    ESP_ERROR_CHECK(esp_wifi_set_csi(1));

    // @See: https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/esp_wifi_types.h#L401
    wifi_csi_config_t configuration_csi;
    configuration_csi.lltf_en = 1;
    configuration_csi.htltf_en = 1;
    configuration_csi.stbc_htltf2_en = 1;
    configuration_csi.ltf_merge_en = 1;
    configuration_csi.channel_filter_en = 1;
    configuration_csi.manu_scale = false;

    ESP_ERROR_CHECK(esp_wifi_set_csi_config(&configuration_csi));
    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&wifi_csi_cb, NULL));
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

    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);

    csi_init();
}

void station_loop() {
    print_csv_header();
    esp_http_client_config_t config = {
            .url = "https://192.168.4.1:80",
            .event_handler = _http_event_handle,
            .is_async = true,
    };
    while (1) {
        while (!(xEventGroupGetBits(s_wifi_event_group) & WIFI_CONNECTED_BIT)) {
            // wait until connected to AP
        }
        esp_http_client_handle_t client = esp_http_client_init(&config);
        esp_http_client_perform(client);

        esp_http_client_cleanup(client);
    }
}

void nvs_init() {
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void app_main() {
    nvs_init();
    station_init();
    station_loop();
}
