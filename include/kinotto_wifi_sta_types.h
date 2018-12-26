#ifndef __KINOTTO_WIFI_STA_TYPES_H__
#define __KINOTTO_WIFI_STA_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <linux/if.h>

#define KINOTTO_WIFI_STA_SSID_LEN 32
#define KINOTTO_WIFI_STA_PSK_LEN 64
#define KINOTTO_WIFI_STA_BSSID_LEN 17

#define KINOTTO_WIFI_STA_SSID_BUF_SIZE (KINOTTO_WIFI_STA_SSID_LEN + 1)
#define KINOTTO_WIFI_STA_BSSID_BUF_SIZE (KINOTTO_WIFI_STA_BSSID_LEN + 1)
#define KINOTTO_WIFI_STA_SECURITY_BUF_SIZE 16

typedef struct kinotto_wifi_sta kinotto_wifi_sta_t;

typedef struct kinotto_wifi_sta_scan_result {
	char ssid[KINOTTO_WIFI_STA_SSID_BUF_SIZE];
	char bssid[KINOTTO_WIFI_STA_BSSID_BUF_SIZE];
	char security[KINOTTO_WIFI_STA_SECURITY_BUF_SIZE];
	int frequency;
	int level;
} kinotto_wifi_sta_scan_result_t;

typedef enum kinotto_wifi_sta_states {
	KINOTTO_WIFI_STA_ERROR,
	KINOTTO_WIFI_STA_DISCONNECTED,
	KINOTTO_WIFI_STA_SCANNING,
	KINOTTO_WIFI_STA_CONNECTING,
	KINOTTO_WIFI_STA_CONNECTED
} kinotto_wifi_sta_states_t;

typedef struct kinotto_wifi_sta_info {
	kinotto_wifi_sta_states_t state;
	kinotto_wifi_sta_scan_result_t sta;
} kinotto_wifi_sta_info_t;

#ifdef __cplusplus
}
#endif

#endif
