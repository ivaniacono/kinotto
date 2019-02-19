/**
 * @file kinotto_wifi_sta_types.h
 * @author Ivan Iacono
 * @brief Kinotto types definitions.
 *
 * This header provides type definitions to use with kinotto wifi sta functions.
 */

#ifndef __KINOTTO_WIFI_STA_TYPES_H__
#define __KINOTTO_WIFI_STA_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <linux/if.h>

/**
 *  SSID length.
 */
#define KINOTTO_WIFI_STA_SSID_LEN 32

/**
 * Station PSK length.
 */
#define KINOTTO_WIFI_STA_PSK_LEN 64

/**
 * Station BSSID string length.
 */
#define KINOTTO_WIFI_STA_BSSID_LEN 17

/**
 * Station SSID vector size.
 */
#define KINOTTO_WIFI_STA_SSID_BUF_SIZE (KINOTTO_WIFI_STA_SSID_LEN + 1)

/**
 * Station BSSID string vector size.
 */
#define KINOTTO_WIFI_STA_BSSID_BUF_SIZE (KINOTTO_WIFI_STA_BSSID_LEN + 1)

/**
 * Station security string vector size.
 */
#define KINOTTO_WIFI_STA_SECURITY_BUF_SIZE 16

/**
 * Kinotto wifi station object.
 */
typedef struct kinotto_wifi_sta kinotto_wifi_sta_t;

/**
 * Structure to contain wifi sta details.
 */
typedef struct kinotto_wifi_sta_detail {
	/*@{*/
	char ssid[KINOTTO_WIFI_STA_SSID_BUF_SIZE]; /**< station SSID */
	char bssid[KINOTTO_WIFI_STA_BSSID_BUF_SIZE]; /**< station bssid string */
	char security[KINOTTO_WIFI_STA_SECURITY_BUF_SIZE]; /**< station security */
	int frequency; /**< station frequency */
	int level; /**< station signal level */
	/*@}*/
} kinotto_wifi_sta_detail_t;

typedef struct kinotto_wifi_sta_connect {
	/*@{*/
	char ssid[KINOTTO_WIFI_STA_SSID_BUF_SIZE]; /**< station SSID */
	char psk[KINOTTO_WIFI_STA_PSK_LEN]; /**< station PSK */
	int timeout; /**< max connection timeout */
	int remove_all; /**< remove existing connection before connecting */
	/*@}*/
} kinotto_wifi_sta_connect_t;

/**
 * Enumaration of station interface states.
 */
typedef enum kinotto_wifi_sta_states {
	/*@{*/
	KINOTTO_WIFI_STA_ERROR, /**< Error */
	KINOTTO_WIFI_STA_DISCONNECTED, /**< Disconnected/Disassociated */
	KINOTTO_WIFI_STA_SCANNING, /**< Scanning for available networks */
	KINOTTO_WIFI_STA_CONNECTING, /**< Connecting to wifi network */
	KINOTTO_WIFI_STA_CONNECTED /**< Connected to wifi network */
	/*@}*/
} kinotto_wifi_sta_states_t;

/**
 * Structure to contain station info.
 */
typedef struct kinotto_wifi_sta_info {
	/*@{*/
	kinotto_wifi_sta_states_t state; /**< station interface state */
	kinotto_wifi_sta_detail_t sta; /**< station status details */
	/*@}*/
} kinotto_wifi_sta_info_t;

#ifdef __cplusplus
}
#endif

#endif
