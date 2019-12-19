#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include <esp_http_server.h>
#include <esp_wifi_internal.h>

int server, client;
#define NUM_RECV 2
char recv_buf[NUM_RECV + 1];
char *input = "1\n";

void socket_listener_loop(bool is_sta, bool (*is_wifi_connected)()) {
    int server = -1;
    while (1) {
        close(server);
        uint32_t inet_len;
        struct sockaddr_in saddr, caddr;

        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(2223);
        saddr.sin_addr.s_addr = htonl(INADDR_ANY);

        while (is_sta && !is_wifi_connected()) {
            // wait until connected to AP
            printf("waiting\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        server = socket(PF_INET, SOCK_DGRAM, 0);  // SOCK_RAW?
        printf("socket: %i\n", server);
        if (server == -1) {
            printf("Socket creation error\n");
            continue;
        }

        if (bind(server, (struct sockaddr *) &saddr, sizeof(struct sockaddr)) == -1) {
            printf("Socket bind error\n");
            continue;
        }

        if (listen(server, 5) == -1) {
            printf("ERROR: Socket listen error [%s]\n", strerror(errno));
            vTaskDelay(10 / portTICK_PERIOD_MS);
            continue;
        }

        inet_len = sizeof(caddr);
        if ((client = accept(server, (struct sockaddr *) &caddr, &inet_len)) == -1) {
            printf("Client accept error\n");
            close(server);
            continue;
        }
        printf("server new client connection [%s/%d]\n", inet_ntoa(caddr.sin_addr), caddr.sin_port);

        while (1) {
            read(client, &recv_buf, NUM_RECV);
            printf("READ_SOMETHING\n");
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

void socket_transmitter_sta_loop(bool (*is_wifi_connected)()) {
    int socket_fd = -1;
    while (1) {
        close(socket_fd);
//        char *ip = "192.168.4.1";
        char *ip = "172.20.10.1";
        struct sockaddr_in caddr;
        caddr.sin_family = AF_INET;
        caddr.sin_port = htons(2223);
        while (!is_wifi_connected()) {
            // wait until connected to AP
            printf("waiting\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        if (inet_aton(ip, &caddr.sin_addr) == 0) {
            printf("ERROR: inet_aton\n");
            continue;
        }

        socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
        if (socket_fd == -1) {
            printf("ERROR: Socket creation error [%s]\n", strerror(errno));
            continue;
        }
        if (connect(socket_fd, (const struct sockaddr *) &caddr, sizeof(struct sockaddr)) == -1) {
            printf("ERROR: socket connection error [%s]\n", strerror(errno));
            continue;
        }

        while (1) {
            if (!is_wifi_connected()) {
                printf("ERROR: wifi is not connected\n");
                break;
            }

            if (!esp_wifi_internal_tx_is_stop()) {
                if (sendto(socket_fd, &input, strlen(input), 0, (const struct sockaddr *) &caddr, sizeof(caddr)) != strlen(input)) {
                    printf("ERROR: failed writing network data [%s]\n", strerror(errno));
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    continue;
                }
            }
        }
    }
}

void socket_transmitter_ap_loop(int (*get_num_clients)()) {
    int socket_fd = -1;
    while (1) {
        close(socket_fd);
        printf("loop\n");
        char *ip = "192.168.4.2";
        struct sockaddr_in caddr;
        caddr.sin_family = AF_INET;
        caddr.sin_port = htons(2223);
        if (inet_aton(ip, &caddr.sin_addr) == 0) {
            continue;
        }
        while (!get_num_clients()) {
            // wait until connected to AP
            printf("waiting\n");
        }

        socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
        if (socket_fd == -1) {
            printf("Socket creation error [%s]\n", strerror(errno));
            continue;
        }
        if (connect(socket_fd, (const struct sockaddr *) &caddr, sizeof(struct sockaddr)) == -1) {
            printf("socket connection error [%s]\n", strerror(errno));
            continue;
        }

        while (1) {
            if (!get_num_clients()) {
                break;
            }

            if (!esp_wifi_internal_tx_is_stop()) {
                printf("writing\n");
                vTaskDelay(1 / portTICK_PERIOD_MS);

                if (sendto(socket_fd, &input, strlen(input), 0, (const struct sockaddr *) &caddr, sizeof(caddr)) != strlen(input)) {
                    printf("ERROR: failed writing network data [%s]\n", strerror(errno));
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                    continue;
                }
            }
        }
    }
}

int last_num_clients = 0;
int *socket_fd;
ip4_addr_t *ip4_addr_t_fd;
wifi_sta_list_t sta;
tcpip_adapter_sta_list_t tcpip_sta_list;

int get_index_for(ip4_addr_t ip) {
    for (int i = 0; i < tcpip_sta_list.num; ++i) {
        if (ip4_addr_t_fd[i].addr == ip.addr) {
            return i;
        }
    }
    return -1;
}

#define ip4_addr_get_byte_val(ipaddr, idx) ((uint8_t)(((ipaddr).addr >> (idx * 8)) & 0xff))
#define ip4_addr4_val(ipaddr) ip4_addr_get_byte_val(ipaddr, 3)

void socket_transmitter_ap_loop_multi_sta(int (*get_num_clients)()) {
    socket_fd = (int *) malloc(sizeof(int *) * 10);
    ip4_addr_t_fd = (ip4_addr_t *) malloc(sizeof(ip4_addr_t * ) * 10);

    while (1) {
        printf("loop (%i)\n", get_num_clients());
        if (last_num_clients != get_num_clients()) {
            // determine new clients
            esp_wifi_ap_get_sta_list(&sta);
            tcpip_adapter_get_sta_list(&sta, &tcpip_sta_list);
            for (int i = 0; i < tcpip_sta_list.num; i++) {
                printf(":::%i\n", i);
                if (get_index_for(tcpip_sta_list.sta[i].ip) == -1) {
                    ip4_addr_t_fd[i] = tcpip_sta_list.sta[i].ip;
                    char ip[16];
                    sprintf(ip, "192.168.4.%i", ip4_addr4_val(tcpip_sta_list.sta[i].ip));
//                    int socket_fd;
                    struct sockaddr_in caddr;
                    caddr.sin_family = AF_INET;
                    caddr.sin_port = htons(2223);
                    if (inet_aton(ip, &caddr.sin_addr) == 0) {
                        printf("SOCKET_ERROR: Some error...\n");
                        continue;
                    }

                    socket_fd[i] = socket(PF_INET, SOCK_DGRAM, 0);
                    if (socket_fd[i] == -1) {
                        printf("SOCKET_ERROR: Socket creation error [%s]\n", strerror(errno));
                        socket_fd[i] = -1;
                        continue;
                    }
                    if (connect(socket_fd[i], (const struct sockaddr *) &caddr, sizeof(struct sockaddr)) == -1) {
                        printf("SOCKET_ERROR: socket connection error [%s]\n", strerror(errno));
                        socket_fd[i] = -1;
                        continue;
                    }
                    last_num_clients++;
                }
            }
        }

        for (int i = 0; i < tcpip_sta_list.num; i++) {
            printf("writing\n");
            if (write(socket_fd[i], &input, strlen(input)) != strlen(input)) {
                printf("error writing network data [%s]\n", strerror(errno));
//                close(socket_fd);
//                break;
            }
        }
    }
}


//void socket_multi_transmitter_ap_loop(int (*get_num_clients)()) {
//    while (1) {
////        esp_wifi_ap_get_sta_list(&sta);
//        for (int i = 0; i < get_num_clients(); i++) {
//            if (socket_fd[i] == NULL) {
//                char *ip = "192.168.4.1";
//                sprintf(ip, "192.168.4.%i", i + 2);
//                struct sockaddr_in caddr;
//                caddr.sin_family = AF_INET;
//                caddr.sin_port = htons(2223);
//                if (inet_aton(ip, &caddr.sin_addr) == 0) {
//                    continue;
//                }
//
//                socket_fd[i] = socket(PF_INET, SOCK_DGRAM, 0);
//                if (socket_fd[i] == -1) {
//                    printf("Socket creation error [%s]\n", strerror(errno));
//                    continue;
//                }
//                if (connect(socket_fd[i], (const struct sockaddr *) &caddr, sizeof(struct sockaddr)) == -1) {
//                    printf("socket connection error [%s]\n", strerror(errno));
//                    continue;
//                }
//            }
//
//            char *input = "000000000000000000000000000000000000000000000000000000000000000000000000000\n"; // 75
//            if (socket_fd[i] != NULL) {
//                printf("sending\n");
//                if (write(socket_fd[i], &input, strlen(input)) != strlen(input)) {
//                    close(socket_fd);
//                }
//            }
//        }
//    }
//}