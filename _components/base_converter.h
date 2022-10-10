//
// Created by Md Touhiduzzaman on 10/10/22.
//
#ifndef ACTIVE_AP_BASE_CONVERTER_H
#define ACTIVE_AP_BASE_CONVERTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX_LENGTH 2

char *dec_to_hex(int decVal) {
    char *hex = (char *) malloc(HEX_LENGTH * sizeof(char));
    for (int i = 0; i < HEX_LENGTH; i++) {
        hex[i] = '0';
    }
    if (decVal < 0 || decVal > 255) {
        printf("E:dec_to_hex: Decimal Value is Out of Range.\n");
        return hex;
    }
    int quo = decVal, i = HEX_LENGTH - 1;
    while (quo != 0) {
        int tmp = quo % 16;
        hex[i--] = tmp + (tmp < 10 ? 48 : 55);
        quo /= 16;
    }
    return hex;
}

int hex_to_dec(char *hex) {
    if (HEX_LENGTH != strlen(hex)) {
        printf("E:hex_to_dec: Incompatible Hexadecimal Number - Size must be 2.\n");
        return 0;
    }
    int d = 0, base = 1;
    for (int i = HEX_LENGTH - 1; i >= 0; i--) {
        int h = hex[i] - ((hex[i] >= '0' && hex[i] <= '9') ? 48 :
                          ((hex[i] >= 'A' && hex[i] <= 'F') ? 55 :
                           ((hex[i] >= 'a' && hex[i] <= 'f') ? 87 : 0)));
        d += (h * base);
        base *= 16;
    }
    return d;
}

char *encode_to_hex(signed int decVal) {
    // CSI r/i values are in between {-128, 128}, so adding 128 will remove make the signed values unsigned &
    // keep these in range [00, FF] in hexadecimal.
    unsigned int d = decVal + 128;
    return dec_to_hex(d);
}

int decode_from_hex(char *hex) {
    return hex_to_dec(hex) - 128;
}

#endif //ACTIVE_AP_BASE_CONVERTER_H