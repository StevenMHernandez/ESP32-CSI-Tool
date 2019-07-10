#ifndef ESP32_CSI_TIME_COMPONENT_H
#define ESP32_CSI_TIME_COMPONENT_H

static char *SET_TIMESTAMP_SIMPLE_TEMPLATE = "%li.%li";
static char *SET_TIMESTAMP_TEMPLATE = "SETTIME: %li.%li";

bool real_time_set = false;

bool match_set_timestamp_template(char *candidate_string) {
    long int tv_sec;
    long int tv_usec;
    return sscanf(candidate_string, SET_TIMESTAMP_TEMPLATE, &tv_sec, &tv_usec) > 0;
}

void time_set(char *timestamp_string) {
    long int tv_sec;
    long int tv_usec;

    int res = sscanf(timestamp_string, SET_TIMESTAMP_TEMPLATE, &tv_sec, &tv_usec);
    if (res <= 0) {
        res = sscanf(timestamp_string, SET_TIMESTAMP_SIMPLE_TEMPLATE, &tv_sec, &tv_usec);
    }

    if (res > 0) {
        struct timeval now = {.tv_sec = tv_sec, .tv_usec = tv_usec};
        settimeofday(&now, NULL);
        real_time_set = true;
    }
}

char *time_string_get() {
    struct timeval currentTimeGot;
    gettimeofday(&currentTimeGot, NULL);
    ssize_t resp_size = snprintf(NULL, 0, "%li.%li", currentTimeGot.tv_sec, currentTimeGot.tv_usec);
    char *resp = malloc(resp_size + 1);
    snprintf(resp, resp_size + 1, "%li.%li", currentTimeGot.tv_sec, currentTimeGot.tv_usec);
    return resp;
}

#endif //ESP32_CSI_TIME_COMPONENT_H
