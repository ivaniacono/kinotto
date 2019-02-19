// support for strndup GNU extension
#define _POSIX_C_SOURCE 200809L

#include "kinotto_wpa_ctrl_wrapper.h"
#include "kinotto_types.h"
#include "kinotto_wifi_sta_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wpa_ctrl.h"

#ifndef CONFIG_CTRL_IFACE_DIR
#define CONFIG_CTRL_IFACE_DIR "/var/run/wpa_supplicant/"
#endif

#define WPA_CTRL_CMD_SIZE 128

static const char *ctrl_iface_dir = CONFIG_CTRL_IFACE_DIR;

struct kinotto_wpa_ctrl_wrapper {
	struct wpa_ctrl *ctrl_conn;
};

static int kinotto_wpa_ctrl_wrapper_cmd(struct wpa_ctrl *ctrl_conn,
					const char *cmd, char *buf, size_t buf_size);
static int kinotto_wpa_ctrl_wrapper_parse_security(const char *flags, int len,
						   char *buf);
static int kinotto_wpa_ctrl_wrapper_ssid_is_hidden(const char *ssid, int len);
static int
kinotto_wpa_ctrl_wrapper_parse_bss(const char *scan_result, int result_size,
				   struct kinotto_wifi_sta_detail *buf);

static int
kinotto_wpa_ctrl_wrapper_parse_wpa_state(const char *wpa_state, int len,
					 kinotto_wifi_sta_states_t *state);

static int
kinotto_wpa_ctrl_wrapper_parse_status(const char *status_result,
				      int result_size,
				      struct kinotto_wifi_sta_info *buf);

kinotto_wpa_ctrl_wrapper_t *
kinotto_wpa_ctrl_wrapper_open_interface(const char *ifname)
{
	char *ctrl_path;
	kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper;

	kinotto_wpa_ctrl_wrapper = malloc(sizeof *kinotto_wpa_ctrl_wrapper);
	if (!kinotto_wpa_ctrl_wrapper)
		goto error_malloc_1;

	ctrl_path = (char *)malloc(
	    (strlen(ctrl_iface_dir) + strlen(ifname) + 1) * sizeof(char));
	if (!ctrl_path)
		goto error_malloc_2;

	strncpy(ctrl_path, ctrl_iface_dir,
		strlen(ctrl_iface_dir) * sizeof(char));
	strncpy(ctrl_path + (strlen(ctrl_iface_dir) * sizeof(char)), ifname,
		KINOTTO_IFSIZE);

	kinotto_wpa_ctrl_wrapper->ctrl_conn = wpa_ctrl_open(ctrl_path);
	if (!kinotto_wpa_ctrl_wrapper->ctrl_conn)
		goto error_wpa_ctrl_open;

	free(ctrl_path);
	return kinotto_wpa_ctrl_wrapper;

error_malloc_1:
	return NULL;

error_malloc_2:
	free(kinotto_wpa_ctrl_wrapper);
	return NULL;

error_wpa_ctrl_open:
	fprintf(stderr,
		"Failed to connect to wpa_supplicant global interface: %s\n",
		ctrl_path);
	free(kinotto_wpa_ctrl_wrapper);
	free(ctrl_path);
	return NULL;
}

void kinotto_wpa_ctrl_wrapper_destroy(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper)
{
	if (kinotto_wpa_ctrl_wrapper) {
		wpa_ctrl_close(kinotto_wpa_ctrl_wrapper->ctrl_conn);
		free(kinotto_wpa_ctrl_wrapper);
	}
}

static int kinotto_wpa_ctrl_wrapper_cmd(struct wpa_ctrl *ctrl_conn,
					const char *cmd, char *buf, size_t buf_size)
{
	int ret;

	memset(buf, 0, buf_size);

	if (!ctrl_conn) {
		fprintf(stderr,
			"Not connected to wpa_supplicant - command dropped.\n");
		goto error;
	}

	ret = wpa_ctrl_request(ctrl_conn, cmd, strlen(cmd) * sizeof(char), buf,
			       &buf_size, NULL);
	if (ret) {
		fprintf(stderr, "'%s' command failed.\n", cmd);
		goto error;
	}

	buf[buf_size - 1] = '\0';

	return 0;

error:
	return -1;
}

int kinotto_wpa_ctrl_wrapper_disconnect_network(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper)
{
	char buf[512] = {0};
	size_t buf_size;

	buf_size = sizeof(buf);

	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 "DISCONNECT", buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	return 0;

error_wpa_ctrl_wrapper:
	return -1;
}

int kinotto_wpa_ctrl_wrapper_connect_network(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper,
	kinotto_wifi_sta_connect_t *kinotto_wifi_sta_connect, int remove_all)
{
	char buf[512] = {0};
	int buf_size = sizeof(buf);
	int network_id = 0;
	char cmd[WPA_CTRL_CMD_SIZE] = {0};

	if (strlen(kinotto_wifi_sta_connect->ssid) > KINOTTO_WIFI_STA_SSID_LEN)
		goto error_ssid;

	if (strlen(kinotto_wifi_sta_connect->psk) > KINOTTO_WIFI_STA_PSK_LEN)
		goto error_psk;

	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 "DISCONNECT", buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	if (remove_all) {
		if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
						 "REMOVE_NETWORK all", buf, buf_size))
			goto error_wpa_ctrl_wrapper;
	}

	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 "ADD_NETWORK", buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	network_id = atoi(buf);

	snprintf(cmd, WPA_CTRL_CMD_SIZE, "SET_NETWORK %d ssid \"%s\"",
		network_id, kinotto_wifi_sta_connect->ssid);
	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 cmd, buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	memset(cmd, 0, 128);
	if (!strlen(kinotto_wifi_sta_connect->psk)) {
		snprintf(cmd, WPA_CTRL_CMD_SIZE, "SET_NETWORK %d key_mgmt NONE",
			 network_id);
	} else {
		snprintf(cmd, WPA_CTRL_CMD_SIZE, "SET_NETWORK %d psk \"%s\"",
			 network_id, kinotto_wifi_sta_connect->psk);
	}
	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 cmd, buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	memset(cmd, 0, 128);
	snprintf(cmd, WPA_CTRL_CMD_SIZE, "ENABLE_NETWORK %d", network_id);
	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 cmd, buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 "RECONNECT", buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	return 0;

error_ssid:
	fprintf(stderr, "Invalid SSID length.\n");
	return -1;

error_psk:
	fprintf(stderr, "Invalid PSK length.\n");
	return -1;

error_wpa_ctrl_wrapper:
	return -1;
}

int kinotto_wpa_ctrl_wrapper_status(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper,
    kinotto_wifi_sta_info_t *sta_info)
{
	char buf[512] = {0};
	int buf_size = sizeof(buf);

	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 "STATUS", buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	if (kinotto_wpa_ctrl_wrapper_parse_status(buf, buf_size, sta_info))
		goto error_wpa_ctrl_wrapper;

	return 0;

error_wpa_ctrl_wrapper:
	return -1;
}

static int kinotto_wpa_ctrl_wrapper_parse_security(const char *flags, int len,
						   char *buf)
{
	char *pch;

	if (!flags)
		goto error_unsupported;

	if (!len)
		goto error_unsupported;

	pch = strstr(flags, "WPA2");
	if (pch) {
		strncpy(buf, pch, 8);
		return 0;
	}

	pch = strstr(flags, "WPA");
	if (pch) {
		strncpy(buf, pch, 7);
		return 0;
	}

	pch = strstr(flags, "WEP");
	if (pch) {
		strncpy(buf, pch, 3);
		return 0;
	}

	/* TODO: match ESS otherwhise throw error? */
	strncpy(buf, "NONE", 4);
	return 0;

error_unsupported:
	fprintf(stderr, "Security method not supported.\n");
	return -1;
}

static int kinotto_wpa_ctrl_wrapper_ssid_is_hidden(const char *ssid, int len)
{
	if (strstr(ssid, "\\x00"))
		return 1;

	return 0;
}

static int
kinotto_wpa_ctrl_wrapper_parse_bss(const char *scan_result, int result_size,
				   struct kinotto_wifi_sta_detail *buf)
{
	char *left;
	char *right;
	char *token;

	token = strtok(strdup(scan_result), "=");
	while (token) {
		left = strdup(token);

		token = strtok(NULL, "\n");

		if (!token)
			return 0;

		right = strdup(token);

		if (!strncmp(left, "bssid", 5)) {
			strncpy(buf->bssid, right, KINOTTO_WIFI_STA_BSSID_LEN);
		} else if (!strncmp(left, "freq", 4)) {
			buf->frequency = atoi(right);
		} else if (!strncmp(left, "level", 5)) {
			buf->level = atoi(right);
		} else if (!strncmp(left, "flags", 5)) {
			if (kinotto_wpa_ctrl_wrapper_parse_security(
				right, strlen(right), buf->security))
				goto error;
		} else if (!strncmp(left, "ssid", 4)) {
			if (kinotto_wpa_ctrl_wrapper_ssid_is_hidden(
				right, strlen(right)))
				strncpy(buf->ssid, "(hidden)", 8);
			else
				strncpy(buf->ssid, right,
					KINOTTO_WIFI_STA_SSID_LEN);
		}

		free(left);
		free(right);

		token = strtok(NULL, "=");
	}

	free(token);

	return 0;

error:
	return -1;
}

// TODO: Rename to parse_sta
static int
kinotto_wpa_ctrl_wrapper_parse_wpa_state(const char *wpa_state, int len,
					 kinotto_wifi_sta_states_t *state)
{
	// WPA-SUPPLICANT states:
	// DISCONNECTED
	// COMPLETED
	// ASSOCIATED
	// SCANNING
	// AUTHENTICATING,
	// 4WAY_HANDSHAKE

	if (!wpa_state)
		goto error;

	if (!len)
		goto error;

	if (!strncmp(wpa_state, "DISCONNECTED", 12)) {
		*state = KINOTTO_WIFI_STA_DISCONNECTED;
		return 0;
	}

	if (!strncmp(wpa_state, "SCANNING", 8)) {
		*state = KINOTTO_WIFI_STA_SCANNING;
		return 0;
	}

	if (!strncmp(wpa_state, "AUTHENTICATING", 14) ||
	    !strncmp(wpa_state, "ASSOCIATED", 10) ||
	    !strncmp(wpa_state, "4WAY_HANDSHAKE", 13)) {
		*state = KINOTTO_WIFI_STA_CONNECTING;
		return 0;
	}

	if (!strncmp(wpa_state, "COMPLETED", 9)) {
		*state = KINOTTO_WIFI_STA_CONNECTED;
		return 0;
	}

	return 0;

	fprintf(stderr, "Unknown state '%s'.\n", wpa_state);
	return -1;

error:
	return -1;
}

static int
kinotto_wpa_ctrl_wrapper_parse_status(const char *status_result,
				      int result_size,
				      struct kinotto_wifi_sta_info *buf)
{
	char *left;
	char *right;
	char *token;

	memset(buf, 0, sizeof(struct kinotto_wifi_sta_info));
	token = strtok(strdup(status_result), "=");
	while (token) {
		left = strdup(token);

		token = strtok(NULL, "\n");

		if (!token)
			return 0;

		right = strdup(token);

		if (!strncmp(left, "bssid", 5)) {
			strncpy(buf->sta.bssid, right,
				KINOTTO_WIFI_STA_BSSID_LEN);
		} else if (!strncmp(left, "freq", 4)) {
			buf->sta.frequency = atoi(right);
		} else if (!strncmp(left, "key_mgmt", 8)) {
			if (kinotto_wpa_ctrl_wrapper_parse_security(
				right, strlen(right), buf->sta.security))
				goto error;
		} else if (!strncmp(left, "ssid", 4)) {
			if (kinotto_wpa_ctrl_wrapper_ssid_is_hidden(
				right, strlen(right)))
				strncpy(buf->sta.ssid, "(hidden)",
					8); // TODO: use a macro for hidden
			else
				strncpy(buf->sta.ssid, right,
					KINOTTO_WIFI_STA_SSID_LEN);
		} else if (!strncmp(left, "wpa_state", 9)) {
			if (kinotto_wpa_ctrl_wrapper_parse_wpa_state(
				right, strlen(right), &buf->state))
				goto error;
		}

		free(left);
		free(right);

		token = strtok(NULL, "=");
	}

	free(token);

	return 0;

error:
	return -1;
}

int kinotto_wpa_ctrl_wrapper_scan_networks(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper,
    struct kinotto_wifi_sta_detail *result_buf, int result_buf_size)
{
	char buf[2048];
	int buf_size;

	char cmd_bss_n[16];

	int i = 0;

	buf_size = sizeof(buf) - 1;

	/* Keep trying while FAIL-BUSY */
	do {
		if (kinotto_wpa_ctrl_wrapper_cmd(
			kinotto_wpa_ctrl_wrapper->ctrl_conn, "SCAN", buf,
			buf_size))
			goto error;

		sleep(1);
	} while (strncmp(buf, "OK", 2));

	while (strlen(buf)) {
		if (result_buf_size < i) {
			goto error_small_buffer;
		}

		snprintf(cmd_bss_n, 8, "BSS %d", i);
		if (kinotto_wpa_ctrl_wrapper_cmd(
			kinotto_wpa_ctrl_wrapper->ctrl_conn, cmd_bss_n, buf,
			buf_size))
			goto error;

		if (kinotto_wpa_ctrl_wrapper_parse_bss(buf, buf_size,
						       &result_buf[i]))
			goto error;

		i++;
	}

	return i - 1;

error:
	return -1;

error_small_buffer:
	fprintf(stderr, "Buffer for kinotto_wifi_sta_detail "
			"is too small.\n");
	return -1;
}

int kinotto_wpa_ctrl_wrapper_save_config(
    kinotto_wpa_ctrl_wrapper_t *kinotto_wpa_ctrl_wrapper)
{
	char buf[512] = {0};
	int buf_size = sizeof(buf);

	if (kinotto_wpa_ctrl_wrapper_cmd(kinotto_wpa_ctrl_wrapper->ctrl_conn,
					 "SAVE_CONFIG", buf, buf_size))
		goto error_wpa_ctrl_wrapper;

	return 0;

error_wpa_ctrl_wrapper:
	return -1;
}
