#include "kinotto_json_utils.h"
#include "kinotto_ip_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int kinotto_wifi_sta_escape_ssid(const char *ssid, size_t ssid_len,
					char *buf, int buf_size);

static int kinotto_wifi_sta_escape_ssid(const char *ssid, size_t ssid_len,
					char *buf, int buf_size)
{
	int i = 0;
	int buf_offset = 0;

	/* Check buffer size is double of ssid buffer plus NULL termitating char
	 */
	if (buf_size < (ssid_len * 2) + 1)
		goto error_small_buffer;

	for (i = 0; i < ssid_len; i++) {
		if ('\\' == ssid[i] || '"' == ssid[i])
			buf[buf_offset++] = '\\';

		buf[buf_offset++] = ssid[i];
	}
	buf[buf_offset] = '\0';

	return 0;

error_small_buffer:
	fprintf(stderr, "Buffer for '%s' is too small.\n", __FUNCTION__);
	return -1;
}

int kinotto_ip_utils_ip_info_json(kinotto_addr_t *kinotto_addr, char *buf,
				  int buf_size)
{
	const char json_model[] = "{"
				  "\"mac_addr\":\"%s\","
				  "\"ipv4\":\"%s\","
				  "\"netmask\":\"%s\""
				  "}";

	if (NULL == kinotto_addr || !buf_size)
		goto error;

	memset(buf, '\0', buf_size);

	snprintf(buf, buf_size, json_model, kinotto_addr->mac_addr,
		 kinotto_addr->ipv4_addr, kinotto_addr->ipv4_netmask);

	return 0;

error:
	return -1;
}

int kinotto_wifi_sta_info_json(kinotto_wifi_sta_info_t *kinotto_wifi_sta_info,
			       char *buf, int buf_size)
{
	const char json_model_connected[] = "{"
					    "\"status\":\"%s\","
					    "\"bssid\":\"%s\","
					    "\"ssid\":\"%s\","
					    "\"security\":\"%s\","
					    "\"frequency\":\"%d\""
					    "}";

	const char json_model_disconnected[] = "{"
					       "\"status\":\"%s\""
					       "}";

	char *escaped_ssid;

	if (NULL == kinotto_wifi_sta_info || !buf_size)
		goto error;

	escaped_ssid =
	    (char *)calloc((KINOTTO_WIFI_STA_SSID_LEN * 2) + 1, sizeof(char));
	if (NULL == escaped_ssid) {
		return -1;
	}

	// Escape SSID
	if (kinotto_wifi_sta_escape_ssid(
		kinotto_wifi_sta_info->sta.ssid,
		strlen(kinotto_wifi_sta_info->sta.ssid), escaped_ssid,
		(KINOTTO_WIFI_STA_SSID_LEN * 2) + 1))
		goto error;

	memset(buf, '\0', buf_size);

	switch (kinotto_wifi_sta_info->state) {
	case KINOTTO_WIFI_STA_ERROR:
	case KINOTTO_WIFI_STA_DISCONNECTED:
	case KINOTTO_WIFI_STA_SCANNING:
	case KINOTTO_WIFI_STA_CONNECTING:
		snprintf(buf, buf_size, json_model_disconnected,
			 "disconnected");
		break;
	case KINOTTO_WIFI_STA_CONNECTED:
		snprintf(buf, buf_size, json_model_connected, "connected",
			 kinotto_wifi_sta_info->sta.bssid,
			 kinotto_wifi_sta_info->sta.ssid,
			 kinotto_wifi_sta_info->sta.security,
			 kinotto_wifi_sta_info->sta.frequency);
		break;
	}

	free(escaped_ssid);

	return 0;

error:
	free(escaped_ssid);
	return -1;
}

int kinotto_wifi_sta_scan_result_json(struct kinotto_wifi_sta_detail *scan_res,
				      int scan_size, char *buf, int buf_size)
{
	int i;
	const char json_model[] = "{"
				  "\"bssid\":\"%s\","
				  "\"ssid\":\"%s\","
				  "\"security\":\"%s\","
				  "\"level\":\"%d\","
				  "\"frequency\":\"%d\""
				  "}";

	char *json;

	const int jsn_elm_size = 144;
	char *jsn_elm;

	char *escaped_ssid;

	int offset = 0;

	if (NULL == scan_res || scan_size <= 0)
		return -1;

	// must be able to contain at least []\0
	json = (char *)calloc(3, sizeof(char));
	if (NULL == json)
		return -1;

	jsn_elm = (char *)calloc(jsn_elm_size, sizeof(char));
	if (NULL == jsn_elm) {
		free(json);
		return -1;
	}

	escaped_ssid =
	    (char *)calloc((KINOTTO_WIFI_STA_SSID_LEN * 2) + 1, sizeof(char));
	if (NULL == escaped_ssid) {
		free(json);
		free(jsn_elm);
		return -1;
	}

	*json = '[';
	offset++;

	for (i = 0; i <= scan_size; i++) {
		// make sure bbsid is valid in the scan result array
		if (strlen(scan_res[i].bssid)) {
			memset(jsn_elm, '\0', jsn_elm_size);
			memset(escaped_ssid, '\0',
			       (KINOTTO_WIFI_STA_SSID_LEN * 2) + 1);

			// Check weather we need to separate the json objects
			// with `,`
			if (offset > 1) {
				jsn_elm[0] = ',';
			}

			// Escape SSID
			if (kinotto_wifi_sta_escape_ssid(
				scan_res[i].ssid, strlen(scan_res[i].ssid),
				escaped_ssid,
				(KINOTTO_WIFI_STA_SSID_LEN * 2) + 1))
				goto error;

			snprintf(
			    (',' == jsn_elm[0]) ? &jsn_elm[1] : &jsn_elm[0],
			    jsn_elm_size - 1, json_model, scan_res[i].bssid,
			    escaped_ssid, scan_res[i].security,
			    scan_res[i].level, scan_res[i].frequency);

			// allocate a new array to contain the json result we're
			// assembling,
			// current json element, ] and \0
			// FIXME: This strategy might cause memory fragmentation
			// problems
			char *json_tmp;
			json_tmp = (char *)calloc(
			    strlen(json) + strlen(jsn_elm) + 2, sizeof(char));
			strncpy(json_tmp, json, strlen(json));
			strncpy(&json_tmp[offset], jsn_elm, strlen(jsn_elm));
			free(json);
			json = json_tmp;
			offset = strlen(json);
		}
	}

	json[offset++] = ']';
	json[offset] = '\0';

	if (buf_size < offset)
		goto error;

	memset(buf, '\0', buf_size);
	strncpy(buf, json, offset);

	free(json);
	free(jsn_elm);
	free(escaped_ssid);

	return 0;

error:
	free(json);
	free(jsn_elm);
	free(escaped_ssid);
	return -1;
}
