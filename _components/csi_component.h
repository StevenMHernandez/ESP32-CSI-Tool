#ifndef ESP32_CSI_CSI_COMPONENT_H
#define ESP32_CSI_CSI_COMPONENT_H

#include "time_component.h"

char *project_type;

void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data) {
    wifi_csi_info_t d = data[0];
    char mac[20] = {0};
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", d.mac[0], d.mac[1], d.mac[2], d.mac[3], d.mac[4], d.mac[5]);

    outprintf("CSI_DATA,");
    outprintf("%s,", project_type);
    outprintf("%s,", mac);

    // https://github.com/espressif/esp-idf/blob/9d0ca60398481a44861542638cfdc1949bb6f312/components/esp_wifi/include/esp_wifi_types.h#L314
    outprintf("%d,", d.rx_ctrl.rssi);
    outprintf("%d,", d.rx_ctrl.rate);
    outprintf("%d,", d.rx_ctrl.sig_mode);
    outprintf("%d,", d.rx_ctrl.mcs);
    outprintf("%d,", d.rx_ctrl.cwb);
    outprintf("%d,", d.rx_ctrl.smoothing);
    outprintf("%d,", d.rx_ctrl.not_sounding);
    outprintf("%d,", d.rx_ctrl.aggregation);
    outprintf("%d,", d.rx_ctrl.stbc);
    outprintf("%d,", d.rx_ctrl.fec_coding);
    outprintf("%d,", d.rx_ctrl.sgi);
    outprintf("%d,", d.rx_ctrl.noise_floor);
    outprintf("%d,", d.rx_ctrl.ampdu_cnt);
    outprintf("%d,", d.rx_ctrl.channel);
    outprintf("%d,", d.rx_ctrl.secondary_channel);
    outprintf("%d,", d.rx_ctrl.timestamp);
    outprintf("%d,", d.rx_ctrl.ant);
    outprintf("%d,", d.rx_ctrl.sig_len);
    outprintf("%d,", d.rx_ctrl.rx_state);

    char* resp = time_string_get();
    outprintf("%d,", real_time_set);
    outprintf("%s,", resp);
    free(resp);

    outprintf("%d,[", data->len);
    int8_t *my_ptr = data->buf;
    for (int i = 0; i < 128; i++) {
        outprintf("%d ", (uint8_t) my_ptr[i]);
//        outprintf("%02x", (uint8_t) my_ptr[i]);
    }
    outprintf("]\n");
    sd_flush();
}

void _print_csi_csv_header() {
    char *header_str = "type,role,mac,rssi,rate,sig_mode,mcs,bandwidth,smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,secondary_channel,local_timestamp,ant,sig_len,rx_state,real_time_set,real_timestamp,len,CSI_DATA\n";
    outprintf(header_str);
}

void csi_init(char *type) {
    project_type = type;

#if SHOULD_COLLECT_CSI
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
    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&_wifi_csi_cb, NULL));

    _print_csi_csv_header();
#endif
}

#endif //ESP32_CSI_CSI_COMPONENT_H
