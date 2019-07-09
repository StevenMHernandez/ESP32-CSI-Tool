#ifndef ESP32_CSI_SD_COMPONENT_H
#define ESP32_CSI_SD_COMPONENT_H

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

FILE *f;
char filename[10] = {0};

char *_sd_pick_next_file() {
    int i = -1;
    struct stat st;
    while (true) {
        i++;
        printf("Checking %i.csv\n", i);
        sprintf(filename, "/sdcard/%i.csv", i);

        if (stat(filename, &st) != 0) {
            break;
        }

        printf("File size: %li\n", st.st_size);
    }

    return filename;
}

void sd_init() {
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = PIN_NUM_MISO;
    slot_config.gpio_mosi = PIN_NUM_MOSI;
    slot_config.gpio_sck = PIN_NUM_CLK;
    slot_config.gpio_cs = PIN_NUM_CS;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = 1,
            .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("sd.h",
                     "Failed to mount filesystem. "
                     "  If you want the card to be formatted, set format_if_mount_failed = true."
            );
        } else {
            ESP_LOGE("sd.h",
                     "Failed to initialize the card (%s). "
                     "  If you do not have an SD card attached, please ignore this message."
                     "  Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    } else {
        sdmmc_card_print_info(stdout, card);

        char *filename = _sd_pick_next_file();
        f = fopen(filename, "a+");
    }
}

/*
 * Printf for both serial AND sd card
 */
void outprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);

    if (f != NULL) {
        vfprintf(f, format, args);
    }

    va_end(args);
}

#endif //ESP32_CSI_SD_COMPONENT_H
