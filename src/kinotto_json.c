#include "kinotto_json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int kinotto_json_wifi_sta_escape_ssid(const char *ssid, size_t ssid_len,
					     char *dest, int n);

static int kinotto_json_wifi_sta_escape_ssid(const char *ssid, size_t ssid_len,
					     char *dest, int n)
{
	int i = 0;
	int buf_offset = 0;

	/* Check buffer size is double of ssid buffer plus NULL termitating char
	 */
	if (n < (ssid_len * 2) + 1)
		goto error_small_buffer;

	for (i = 0; i < ssid_len; i++) {
		if ('\\' == ssid[i] || '"' == ssid[i])
			dest[buf_offset++] = '\\';

		dest[buf_offset++] = ssid[i];
	}
	dest[buf_offset] = '\0';

	return 0;

error_small_buffer:
	fprintf(stderr, "Buffer for '%s' is too small.\n", __FUNCTION__);
	return -1;
}

int kinotto_json_ip_info(kinotto_info_t *src, char *dest, int n)
{
	const char json_model[] = "{"
				  "\"ifname\":\"%s\","
				  "\"mac_addr\":\"%s\","
				  "\"ipv4\":\"%s\","
				  "\"netmask\":\"%s\""
				  "}";

	if (NULL == src || !n)
		goto error;

	memset(dest, '\0', n);

	snprintf(dest, n, json_model, src->ifname, src->addr.mac_addr,
		 src->addr.ipv4_addr, src->addr.ipv4_netmask);

	return 0;

error:
	return -1;
}

int kinotto_json_ifaces_list(kinotto_info_t *src, int src_n, char *dest, int n)
{
	char json_entry[144]; // TODO: define json_entry size
	int i = 0;
	int j = 0;

	if (!src || !n)
		goto error;

	memset(dest, '\0', n);

	dest[j++] = '[';

	for (i = 0; i < src_n; i++) {
		if (kinotto_json_ip_info(&src[i], json_entry,
					 sizeof(json_entry)))
			goto error;

		int json_len = strlen(json_entry);
		if (i)
			dest[j++] = ',';

		if (n > (j + json_len)) {
			strncpy(&dest[j], json_entry, json_len);
			j += json_len;
		}
	}
	dest[j] = ']';

	return 0;

error:
	return -1;
}

int kinotto_json_sta_info(kinotto_wifi_sta_info_t *src, char *dest, int n)
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

	if (NULL == src || !n)
		goto error;

	// TODO: do not use heap memory

	escaped_ssid =
	    (char *)calloc((KINOTTO_WIFI_STA_SSID_LEN * 2) + 1, sizeof(char));
	if (NULL == escaped_ssid) {
		return -1;
	}

	// Escape SSID
	if (kinotto_json_wifi_sta_escape_ssid(
		src->sta.ssid, strlen(src->sta.ssid), escaped_ssid,
		(KINOTTO_WIFI_STA_SSID_LEN * 2) + 1))
		goto error;

	memset(dest, '\0', n);

	switch (src->state) {
	case KINOTTO_WIFI_STA_ERROR:
	case KINOTTO_WIFI_STA_DISCONNECTED:
	case KINOTTO_WIFI_STA_SCANNING:
	case KINOTTO_WIFI_STA_CONNECTING:
		snprintf(dest, n, json_model_disconnected, "disconnected");
		break;
	case KINOTTO_WIFI_STA_CONNECTED:
		snprintf(dest, n, json_model_connected, "connected",
			 src->sta.bssid, src->sta.ssid, src->sta.security,
			 src->sta.frequency);
		break;
	}

	free(escaped_ssid);

	return 0;

error:
	free(escaped_ssid);
	return -1;
}

int kinotto_json_sta_scan_result(struct kinotto_wifi_sta_detail *scan_res,
				 int scan_n, char *dest, int n)
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

	if (NULL == scan_res || scan_n <= 0)
		return -1;

	// TODO: do not use heap memory

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

	for (i = 0; i <= scan_n; i++) {
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
			if (kinotto_json_wifi_sta_escape_ssid(
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
			// TODO: Do not use heap memory
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

	if (n < offset)
		goto error;

	memset(dest, '\0', n);
	strncpy(dest, json, offset);

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
