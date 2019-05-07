#include "kinotto_wifi_sta.h"
#include "kinotto_wpa_ctrl_wrapper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct kinotto_wifi_sta {
	kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper;
};

kinotto_wifi_sta_t *kinotto_wifi_sta_init(const char *ifname)
{
	kinotto_wifi_sta_t *kinotto_wifi_sta = malloc(sizeof *kinotto_wifi_sta);
	if (!kinotto_wifi_sta)
		goto error_malloc;

	kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper =
	    kinotto_wpa_ctrl_wrapper_open_interface(ifname);
	if (!kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper)
		goto error_malloc;

	return kinotto_wifi_sta;

error_malloc:
	kinotto_wifi_sta_destroy(kinotto_wifi_sta);
	return NULL;
}

void kinotto_wifi_sta_destroy(kinotto_wifi_sta_t *kinotto_wifi_sta)
{
	if (kinotto_wifi_sta) {
		kinotto_wpa_ctrl_wrapper_destroy(
		    kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper);

		free(kinotto_wifi_sta);
	}
}

int kinotto_wifi_sta_get_info(kinotto_wifi_sta_t *kinotto_wifi_sta,
			      kinotto_wifi_sta_info_t *kinotto_wifi_sta_info)
{
	if (kinotto_wpa_ctrl_wrapper_status(
		kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper,
		kinotto_wifi_sta_info))
		return -1;

	return 0;
}

int kinotto_wifi_sta_connect_network(
    kinotto_wifi_sta_t *kinotto_wifi_sta,
    kinotto_wifi_sta_info_t *kinotto_wifi_sta_info,
    kinotto_wifi_sta_connect_t *kinotto_wifi_sta_connect)
{
	if (kinotto_wifi_sta_connect->timeout < 0)
		return -1;

	if (kinotto_wpa_ctrl_wrapper_connect_network(
		kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper,
		kinotto_wifi_sta_connect, kinotto_wifi_sta_connect->remove_all))
		goto error_wpa_ctrl_wrapper;

	do {
		if (kinotto_wpa_ctrl_wrapper_status(
			kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper,
			kinotto_wifi_sta_info))
			goto error_wpa_ctrl_wrapper;
		sleep(1);
	} while ((--kinotto_wifi_sta_connect->timeout) &&
		 (kinotto_wifi_sta_info->state != KINOTTO_WIFI_STA_CONNECTED));

	if (kinotto_wifi_sta_info->state != KINOTTO_WIFI_STA_CONNECTED) {
		kinotto_wpa_ctrl_wrapper_disconnect_network(
		    kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper);
		goto error_connect_timeout;
	}

	return 0;

error_wpa_ctrl_wrapper:
	return -1;

error_connect_timeout:
	return -1;
}

int kinotto_wifi_sta_disconnect_network(
    kinotto_wifi_sta_t *kinotto_wifi_sta,
    kinotto_wifi_sta_info_t *kinotto_wifi_sta_info)
{
	if (kinotto_wpa_ctrl_wrapper_disconnect_network(
		kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper))
		goto error_wpa_ctrl_wrapper;

	if (kinotto_wpa_ctrl_wrapper_status(
		kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper,
		kinotto_wifi_sta_info))
		goto error_wpa_ctrl_wrapper;

	return 0;

error_wpa_ctrl_wrapper:
	return -1;
}

int kinotto_wifi_sta_scan_networks(kinotto_wifi_sta_t *kinotto_wifi_sta,
				   kinotto_wifi_sta_detail_t *buf, int buf_size)
{
	int ret;
	int i = 0;
	int retries = 2;

	for (i = 0; i < buf_size; i++) {
		// TODO: just memset the whole struct
		memset(buf[i].ssid, '\0', KINOTTO_WIFI_STA_SSID_BUF_SIZE);
		memset(buf[i].bssid, '\0', KINOTTO_WIFI_STA_BSSID_BUF_SIZE);
		memset(buf[i].security, '\0',
		       KINOTTO_WIFI_STA_SECURITY_BUF_SIZE);
		buf[i].frequency = 0;
		buf[i].level = 0;
	}

	do {
		ret = kinotto_wpa_ctrl_wrapper_scan_networks(
		    kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper, buf, buf_size);
		sleep(1);
	} while (!ret && --retries);

	if (-1 == ret)
		goto error;

	return ret;

error:
	return -1;
}

int kinotto_wifi_sta_save_config(kinotto_wifi_sta_t *kinotto_wifi_sta)
{
	// TODO: we want to use our config file here and not the wpa_supplicant
	// one
	// for ip related stuff create another file under kinotto_ip_utils
	if (kinotto_wpa_ctrl_wrapper_save_config(
		kinotto_wifi_sta->kinotto_wpa_ctrl_wrapper))
		goto error;

	return 0;

error:
	return -1;
}
