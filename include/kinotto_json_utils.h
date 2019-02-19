/**
 * @file kinotto_json_utils.h
 * @author Ivan Iacono
 * @brief Kinotto JSON utils.
 *
 * This header provides prototypes for getting kinotto results in JSON.
 */

#ifndef __KINOTTO_JSON_UTILS_H__
#define __KINOTTO_JSON_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_types.h"
#include "kinotto_wifi_sta.h"

/**
 * @brief Get IP info.
 *
 * Get IP info about a specified interface in JSON.
 *
 * @code
 * #define JSON_RES_BUF_SIZE 144 * 1024
 * ...
 * kinotto_addr_t kinotto_addr = {0};
 * char json_res[JSON_RES_BUF_SIZE];
 *
 * if (kinotto_ip_utils_get_ipv4("wlan0", &kinotto_addr)) {
 *  return -1;
 * }
 *
 * kinotto_ip_utils_ip_info_json(&kinotto_addr, json_res, JSON_RES_BUF_SIZE);
 * printf("%s\n", json_res);
 * @endcode
 *
 * @param kinotto_addr pointer to a kinotto_addr_t type.
 * @param buf pointer to buffer where the JSON output is stored.
 * @param buf_size size of the output buffer.
 * @return 0 on success, -1 on failure
 */
int kinotto_ip_utils_ip_info_json(kinotto_addr_t *kinotto_addr, char *buf,
				  int buf_size);

/**
 * @brief Get wifi sta info
 *
 * Get info about a specified wifi sta interface in JSON.
 *
 * @code
 * #define JSON_RES_BUF_SIZE 144 * 1024
 * ...
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * kinotto_wifi_sta_info_t kinotto_wifi_sta_info;
 * char json_res[JSON_RES_BUF_SIZE];
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 *  return -1;
 *
 * kinotto_wifi_sta_get_info(kinotto_wifi_sta, &kinotto_wifi_sta_info);
 * if (kinotto_wifi_sta_info_json(&kinotto_wifi_sta_info, json_res, JSON_RES_BUF_SIZE))
 *  return -1;
 * printf("%s\n", json_res);
 *
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_addr pointer to a kinotto_addr_t type.
 * @param buf pointer to buffer where the JSON output is stored.
 * @param buf_size size of the output buffer.
 * @return 0 on success, -1 on failure
 */
int kinotto_wifi_sta_info_json(kinotto_wifi_sta_info_t *kinotto_wifi_sta_info,
			       char *buf, int buf_size);

/**
 * @brief Get wifi scan result.
 *
 * Get wifi scan result of a specified interface in JSON.
 *
 * @code
 * #define JSON_RES_BUF_SIZE 144 * 1024
 * ...
 * int networks = 0;
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * kinotto_wifi_sta_detail_t scan_result[1024];
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * char json_res[JSON_RES_BUF_SIZE];
 *
 * if (!kinotto_wifi_sta)
 *  return -1;
 *
 * networks = kinotto_wifi_sta_scan_networks(kinotto_wifi_sta, scan_result,
 *              sizeof(scan_result) / sizeof(scan_result[0]));
 * if (-1 == networks)
 *  return -1;
 *
 * if (kinotto_wifi_sta_scan_result_json(scan_result, networks, json_res,
 *      JSON_RES_BUF_SIZE))
 *  return -1;
 * printf("%s\n", json_res);
 *
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_addr pointer to a kinotto_addr_t type.
 * @param buf pointer to buffer where the JSON output is stored.
 * @param buf_size size of the output buffer.
 * @return 0 on success, -1 on failure
 */
int kinotto_wifi_sta_scan_result_json(
    struct kinotto_wifi_sta_detail *scan_res, int scan_size, char *buf,
    int buf_size);

#ifdef __cplusplus
}
#endif

#endif
