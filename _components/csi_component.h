#ifndef ESP32_CSI_CSI_COMPONENT_H
#define ESP32_CSI_CSI_COMPONENT_H

#include "time_component.h"
#include "math.h"
#include <string>
#include <sstream>
#include <iostream>

char *project_type;

#define CSI_RAW 1
#define CSI_AMPLITUDE 0
#define CSI_PHASE 0

#ifndef CONFIG_PRINT_METADATA
#define CONFIG_PRINT_METADATA 1
#endif
#define PRINT_METADATA CONFIG_PRINT_METADATA

#ifndef CONFIG_PRINT_CSI_AS_HEX
#define CONFIG_PRINT_CSI_AS_HEX 0
#endif
#define PRINT_CSI_AS_HEX CONFIG_PRINT_CSI_AS_HEX

std::string lookup_uint8[256] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "100", "101", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120", "121", "122", "123", "124", "125", "126", "127", "128", "129", "130", "131", "132", "133", "134", "135", "136", "137", "138", "139", "140", "141", "142", "143", "144", "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155", "156", "157", "158", "159", "160", "161", "162", "163", "164", "165", "166", "167", "168", "169", "170", "171", "172", "173", "174", "175", "176", "177", "178", "179", "180", "181", "182", "183", "184", "185", "186", "187", "188", "189", "190", "191", "192", "193", "194", "195", "196", "197", "198", "199", "200", "201", "202", "203", "204", "205", "206", "207", "208", "209", "210", "211", "212", "213", "214", "215", "216", "217", "218", "219", "220", "221", "222", "223", "224", "225", "226", "227", "228", "229", "230", "231", "232", "233", "234", "235", "236", "237", "238", "239", "240", "241", "242", "243", "244", "245", "246", "247", "248", "249", "250", "251", "252", "253", "254", "255"};
std::string lookup_int8[256] = {"-128", "-127", "-126", "-125", "-124", "-123", "-122", "-121", "-120", "-119", "-118", "-117", "-116", "-115", "-114", "-113", "-112", "-111", "-110", "-109", "-108", "-107", "-106", "-105", "-104", "-103", "-102", "-101", "-100", "-99", "-98", "-97", "-96", "-95", "-94", "-93", "-92", "-91", "-90", "-89", "-88", "-87", "-86", "-85", "-84", "-83", "-82", "-81", "-80", "-79", "-78", "-77", "-76", "-75", "-74", "-73", "-72", "-71", "-70", "-69", "-68", "-67", "-66", "-65", "-64", "-63", "-62", "-61", "-60", "-59", "-58", "-57", "-56", "-55", "-54", "-53", "-52", "-51", "-50", "-49", "-48", "-47", "-46", "-45", "-44", "-43", "-42", "-41", "-40", "-39", "-38", "-37", "-36", "-35", "-34", "-33", "-32", "-31", "-30", "-29", "-28", "-27", "-26", "-25", "-24", "-23", "-22", "-21", "-20", "-19", "-18", "-17", "-16", "-15", "-14", "-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "100", "101", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "120", "121", "122", "123", "124", "125", "126", "127"};
std::string lookup_uint8_hex[256] = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5A", "5B", "5C", "5D", "5E", "5F", "60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D", "6E", "6F", "70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7A", "7B", "7C", "7D", "7E", "7F", "80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B", "8C", "8D", "8E", "8F", "90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "AA", "AB", "AC", "AD", "AE", "AF", "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD", "BE", "BF", "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF", "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB", "DC", "DD", "DE", "DF", "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF", "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "FA", "FB", "FC", "FD", "FE", "FF"};

std::string output (1024, 0);

void _wifi_csi_cb(void *ctx, wifi_csi_info_t *data) {
    output.clear();

    wifi_csi_info_t d = data[0];
    char mac[20] = {0};
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", d.mac[0], d.mac[1], d.mac[2], d.mac[3], d.mac[4], d.mac[5]);

    output += "CSI_DATA,";
    output += project_type;
    output += ",";

#if PRINT_METADATA
    // https://github.com/espressif/esp-idf/blob/9d0ca60398481a44861542638cfdc1949bb6f312/components/esp_wifi/include/esp_wifi_types.h#L314
    output += mac;
    output += ",";
    output += lookup_int8[d.rx_ctrl.rssi + 128] + ",";
    output += lookup_uint8[d.rx_ctrl.rate] + ",";
    output += lookup_uint8[d.rx_ctrl.sig_mode] + ",";
    output += lookup_uint8[d.rx_ctrl.mcs] + ",";
    output += lookup_uint8[d.rx_ctrl.cwb] + ",";
    output += lookup_uint8[d.rx_ctrl.smoothing] + ",";
    output += lookup_uint8[d.rx_ctrl.not_sounding] + ",";
    output += lookup_uint8[d.rx_ctrl.aggregation] + ",";
    output += lookup_uint8[d.rx_ctrl.stbc] + ",";
    output += lookup_uint8[d.rx_ctrl.fec_coding] + ",";
    output += lookup_uint8[d.rx_ctrl.sgi] + ",";
    output += lookup_int8[d.rx_ctrl.noise_floor + 128] + ",";
    output += lookup_uint8[d.rx_ctrl.ampdu_cnt] + ",";
    output += lookup_uint8[d.rx_ctrl.channel] + ",";
    output += lookup_uint8[d.rx_ctrl.secondary_channel] + ",";
    output += std::to_string(d.rx_ctrl.timestamp) + ",";
    output += lookup_uint8[d.rx_ctrl.ant] + ",";
    output += lookup_uint8[d.rx_ctrl.sig_len] + ",";
    output += lookup_uint8[d.rx_ctrl.rx_state] + ",";
    output += std::to_string(real_time_set) + ",";
    output += std::to_string(get_steady_clock_timestamp()) + ",";
    output += std::to_string(data->len) + ",[";
#else
    output += "[";
#endif

#if CONFIG_SHOULD_COLLECT_ONLY_LLTF
    int data_len = 128;
#else
    int data_len = data->len;
#endif

    int8_t *my_ptr;
#if CSI_RAW
    my_ptr = data->buf;
    for (int i = 0; i < data_len; i++) {
#if PRINT_CSI_AS_HEX
        output += lookup_uint8_hex[(int) my_ptr[i] + 128];
#else
        output += lookup_int8[(int) my_ptr[i]] + " ";
#endif
    }
#endif
#if CSI_AMPLITUDE
    my_ptr = data->buf;
    for (int i = 0; i < data_len / 2; i++) {
        output += std::to_string((int) sqrt(pow(my_ptr[i * 2], 2) + pow(my_ptr[(i * 2) + 1], 2))) + " ";
    }
#endif
#if CSI_PHASE
    my_ptr = data->buf;
    for (int i = 0; i < data_len / 2; i++) {
        output += std::to_string((int) atan2(my_ptr[i*2], my_ptr[(i*2)+1])) + " ";
    }
#endif
    output += "]\n";

    printf(output.c_str());
}

void _print_csi_csv_header() {
#if PRINT_METADATA
    char *header_str = (char *) "type,role,mac,rssi,rate,sig_mode,mcs,bandwidth,smoothing,not_sounding,aggregation,stbc,fec_coding,sgi,noise_floor,ampdu_cnt,channel,secondary_channel,local_timestamp,ant,sig_len,rx_state,real_time_set,real_timestamp,len,CSI_DATA\n";
#else
    char *header_str = (char *) "type,role,CSI_DATA\n";
#endif
    outprintf(header_str);
}

void csi_init(char *type) {
    project_type = type;

#ifdef CONFIG_SHOULD_COLLECT_CSI
    ESP_ERROR_CHECK(esp_wifi_set_csi(1));

    // @See: https://github.com/espressif/esp-idf/blob/master/components/esp_wifi/include/esp_wifi_types.h#L401
    wifi_csi_config_t configuration_csi;
    configuration_csi.lltf_en = 1;
    configuration_csi.htltf_en = 1;
    configuration_csi.stbc_htltf2_en = 1;
    configuration_csi.ltf_merge_en = 1;
    configuration_csi.channel_filter_en = 0;
    configuration_csi.manu_scale = 0;

    ESP_ERROR_CHECK(esp_wifi_set_csi_config(&configuration_csi));
    ESP_ERROR_CHECK(esp_wifi_set_csi_rx_cb(&_wifi_csi_cb, NULL));

    _print_csi_csv_header();
#endif
}

#endif //ESP32_CSI_CSI_COMPONENT_H
