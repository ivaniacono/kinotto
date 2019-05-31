/**
 * @file kinotto_wifi_sta.h
 * @author Ivan Iacono
 * @brief Kinotto wifi station operations.
 *
 * This header provides prototypes for wifi station operations.
 */

#ifndef __KINOTTO_WIFI_STA_H__
#define __KINOTTO_WIFI_STA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_wifi_sta_types.h"
#include <stddef.h>

/**
 * @brief Initialize wifi interface.
 *
 * Establish a connection with wpa_supplicant. This must be called before using
 * station functionalities.
 *
 * @code
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 * 	return -1;
 * @endcode
 *
 * @param ifname wifi interface to use.
 * @return On success returns a pointer to a kinotto_wifi_sta_t, On failure
 * NULL.
 */
kinotto_wifi_sta_t *kinotto_wifi_sta_init(const char *ifname);

/**
 * @brief Free wifi interface.
 *
 * Close connection with wpa_supplicant. This must be called to free resources
 * previously allocated by kinotto_wifi_sta_init().
 *
 * @code
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 * 	return -1;
 *
 * // do something
 * ...
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_wifi_sta pointer to kinotto_wifi_sta_t object.
 */
void kinotto_wifi_sta_destroy(kinotto_wifi_sta_t *kinotto_wifi_sta);

/**
 * @brief Scan for wifi networks.
 *
 * Scan for wifi networks and copy result into a vector of type
 * kinotto_wifi_sta_detail_t.
 *
 * @code
 * int networks = 0;
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * kinotto_wifi_sta_detail_t scan_result[1024];
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 * 	return -1;
 *
 * networks = kinotto_wifi_sta_scan_networks(kinotto_wifi_sta, scan_result,
 *		sizeof(scan_result) / sizeof(scan_result[0]));
 *
 * if (-1 == networks)
 * 	return -1;
 *
 * // do something like print the scan result
 * ...
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_wifi_sta pointer to a kinotto_wifi_sta object.
 * @param dest buffer where to copy result.
 * @param n size of the dest buffer.
 * @return number of wifi networks found, -1 on error.
 */
int kinotto_wifi_sta_scan_networks(kinotto_wifi_sta_t *kinotto_wifi_sta,
				   struct kinotto_wifi_sta_detail *dest,
				   int n);

/**
 * @brief Get wifi station info.
 *
 * Copy wifi station information and copy the result into a buffer of type
 * kinotto_wifi_sta_info_t.
 *
 * @code
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * kinotto_wifi_sta_info_t kinotto_wifi_sta_info;
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 * 	return -1;
 *
 * kinotto_wifi_sta_get_info(kinotto_wifi_sta, &kinotto_wifi_sta_info);
 * ...
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_wifi_sta pointer to a kinotto_wifi_sta object.
 * @param dest buffer where to copy result.
 * @return 0 on success, -1 on error.
 */
int kinotto_wifi_sta_get_info(kinotto_wifi_sta_t *kinotto_wifi_sta,
			      kinotto_wifi_sta_info_t *dest);

/**
 * @brief Connect to a wifi network.
 *
 * Connect to a wifi SSID protected by WPA/WPA2 or NONE (WEP not supported).
 * Network details of the network must be provided in a
 * kinotto_wifi_sta_connect_t
 * struct.
 *
 * @code
 * int rc = 0;
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * kinotto_wifi_sta_info_t result;
 * kinotto_wifi_sta_connect_t network_details;
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 * 	return -1;
 *
 * memset(&network_details, 0, sizeof(kinotto_wifi_sta_connect_t));
 * strncpy(network_details.ssid, "your_ssid", KINOTTO_WIFI_STA_SSID_LEN);
 * strncpy(network_details.psk, "your_psk_key", KINOTTO_WIFI_STA_PSK_LEN);
 * network_details.timeout = WIFI_STA_CONNECT_TIMEOUT_S;
 *
 * rc = kinotto_wifi_sta_connect_network(
 *  kinotto_wifi_sta, &result,
 *  &network_details);
 *
 * if (rc) {
 * 	printf("failed\n");
 * }
 *
 * // at this point you need to get an IP address
 * ...
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_wifi_sta pointer to a kinotto_wifi_sta_t object.
 * @param result buffer where to copy the result.
 * @param network_details pointer to a kinotto_wifi_sta_connect_t
 *  containig connection parameters.
 * @return 0 on success, -1 on error.
 */
int kinotto_wifi_sta_connect_network(
    kinotto_wifi_sta_t *kinotto_wifi_sta,
    kinotto_wifi_sta_info_t *result,
    kinotto_wifi_sta_connect_t *network_details);

/**
 * @brief Disconnect from a wifi network.
 *
 * Disconnect from a wifi station.
 *
 * @code
 * int rc = 0;
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * kinotto_wifi_sta_info_t kinotto_wifi_sta_info;
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 * 	return -1;
 *
 * rc = kinotto_wifi_sta_disconnect_network(kinotto_wifi_sta,
 *                                          &kinotto_wifi_sta_info);
 *
 * if (rc) {
 * 	printf("failed\n");
 * }
 *
 * // at this point you need to flush the IP address
 * ...
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_wifi_sta pointer to a kinotto_wifi_sta_t object.
 * @param kinotto_wifi_sta_info buffer where to copy the result.
 *  containig connection parameters.
 * @return 0 on success, -1 on error.
 */
int kinotto_wifi_sta_disconnect_network(
    kinotto_wifi_sta_t *kinotto_wifi_sta,
    kinotto_wifi_sta_info_t *result);

/**
 * @brief Save wifi station network information.
 *
 * Save wifi station network information in the wpa_supplicant config file. If
 * using wpa_supplicant, the config file must containt the line
 * `update_config=1`.
 *
 * @code
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * // do something like connect to a network
 * ...
 * if (kinotto_wifi_sta_save_config(kinotto_wifi_sta)) {
 * 	printf("failed\n");
 * }
 *
 * @endcode
 */
int kinotto_wifi_sta_save_config(kinotto_wifi_sta_t *kinotto_wifi_sta);

#ifdef __cplusplus
}
#endif

#endif
