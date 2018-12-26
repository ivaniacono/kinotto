#ifndef __KINOTTO_WIFI_STA_H__
#define __KINOTTO_WIFI_STA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_wifi_sta_types.h"
#include <stddef.h>

#define KINOTTO_WIFI_STA_CONNECT_TIMEOUT_S 10

kinotto_wifi_sta_t *kinotto_wifi_sta_init(const char *ctrl_path);

void kinotto_wifi_sta_destroy(kinotto_wifi_sta_t *kinotto_wifi_sta);

int kinotto_wifi_sta_scan_networks(kinotto_wifi_sta_t *kinotto_wifi_sta,
				   struct kinotto_wifi_sta_scan_result *buf,
				   int buf_size);

int kinotto_wifi_sta_get_info(kinotto_wifi_sta_t *kinotto_wifi_sta,
			    kinotto_wifi_sta_info_t *kinotto_wifi_sta_info);

int kinotto_wifi_sta_connect_network(
    kinotto_wifi_sta_t *kinotto_wifi_sta,
    kinotto_wifi_sta_info_t *kinotto_wifi_sta_info, char *ssid, int ssid_len,
    char *psk, int psk_len, int timeout);

int kinotto_wifi_sta_scan_networks(kinotto_wifi_sta_t *kinotto_wifi_sta,
				   struct kinotto_wifi_sta_scan_result *buf,
				   int buf_size);

int kinotto_wifi_sta_save_config(kinotto_wifi_sta_t *kinotto_wifi_sta);

#ifdef __cplusplus
}
#endif

#endif
