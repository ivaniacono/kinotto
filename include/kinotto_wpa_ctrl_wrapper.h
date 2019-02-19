/**
 * @file kinotto_wpa_ctrl_wrapper.h
 * @author Ivan Iacono
 * @brief Kinotto wpa_supplicant wrapper.
 *
 * This header provides prototypes for interacting with wpa_supplicant.
 */

#ifndef __KINOTTO_WPA_CTRL_WRAPPER_H__
#define __KINOTTO_WPA_CTRL_WRAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_wifi_sta_types.h"
#include <stddef.h>

typedef struct kinotto_wpa_ctrl_wrapper kinotto_wpa_ctrl_wrapper_t;

kinotto_wpa_ctrl_wrapper_t *
kinotto_wpa_ctrl_wrapper_open_interface(const char *ifname);

void kinotto_wpa_ctrl_wrapper_destroy(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper);

int kinotto_wpa_ctrl_wrapper_disconnect_network(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper);

int kinotto_wpa_ctrl_wrapper_connect_network(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper,
	kinotto_wifi_sta_connect_t *kinotto_wifi_sta_connect, int remove_all);

int kinotto_wpa_ctrl_wrapper_scan_networks(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper,
    struct kinotto_wifi_sta_detail *result_buf, int result_buf_size);

int kinotto_wpa_ctrl_wrapper_status(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper,
    kinotto_wifi_sta_info_t *sta_info);

int kinotto_wpa_ctrl_wrapper_save_config(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper);

#ifdef __cplusplus
}
#endif

#endif
