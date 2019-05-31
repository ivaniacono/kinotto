/**
 * @file kinotto_json.h
 * @author Ivan Iacono
 * @brief Kinotto JSON utils.
 *
 * This header provides prototypes for getting kinotto results in JSON.
 */

#ifndef __KINOTTO_JSON_H__
#define __KINOTTO_JSON_H__

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
 * kinotto_info_t kinotto_info = {"eth0", {0}};
 * char json_res[JSON_RES_BUF_SIZE] = {0};
 *
 * if (kinotto_net_get_ipv4(kinotto_info.ifname, &kinotto_info->addr)) {
 *  return -1;
 * }
 *
 * kinotto_json_ip_info(&kinotto_info, json_res, JSON_RES_BUF_SIZE);
 * printf("%s\n", json_res);
 * @endcode
 *
 * @param src pointer to a kinotto_info_t type to format.
 * @param dest pointer to buffer where the JSON output is stored.
 * @param n size of the output buffer.
 * @return 0 on success, -1 on failure
 */
int kinotto_json_ip_info(kinotto_info_t *src, char *dest, int n);

/**
 * @brief Get IP all interfaces info
 *
 * Get IP info about all interfaces in JSON.
 *
 * @code
 * #define JSON_RES_BUF_SIZE 144 * 1024
 * #define MAX_IFACES 12
 * ...
 * kinotto_info_t kinotto_info[MAX_IFACES] = {0};
 * kinotto_addr_t *kinotto_addr = &kinotto_info.addr;
 * char json_res[JSON_RES_BUF_SIZE] = {0};
 * int ifaces = 0;
 * int i = 0;
 *
 * ifaces = kinotto_if_get_ifaces(kinotto_info, MAX_IFACES);
 * if (!ifaces)
 *  return -1;
 *
 * for (i = 0; i < ifaces; i++) {
 *  if (kinotto_if_get_mac(kinotto_info.ifname, kinotto_addr)) {
 *   return -1;
 *  }
 *
 *  if (kinotto_if_get_mac(kinotto_info.ifname, kinotto_addr)) {
 *   return -1;
 *  }
 *  if (kinotto_net_get_ipv4(kinotto_info.ifname, kinotto_addr)) {
 *   return -1;
 *  }
 * }
 *
 * kinotto_json_ifaces_list(kinotto_info, n, json_res, JSON_RES_BUF_SIZE);
 * printf("%s\n", json_res);
 * @endcode
 *
 * @param src pointer to a kinotto_info_t type.
 * @param src_n size of the src buffer.
 * @param dest pointer to buffer where the JSON output is stored.
 * @param n size of the output buffer.
 * @return 0 on success, -1 on failure
 */
int kinotto_json_ifaces_list(kinotto_info_t *src, int src_n, char *dest, int n);

/**
 * @brief Get wifi sta info
 *
 * Get info about a specified wifi sta interface in JSON.
 *
 * @code
 * #define JSON_RES_BUF_SIZE 144 * 1024
 * ...
 * kinotto_wifi_sta_t *kinotto_wifi_sta;
 * kinotto_wifi_sta_info_t src = {0};
 * char json_res[JSON_RES_BUF_SIZE] = {0};
 *
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * if (!kinotto_wifi_sta)
 *  return -1;
 *
 * kinotto_wifi_sta_get_info(kinotto_wifi_sta, &src);
 * if (kinotto_json_sta_info(&src, json_res, JSON_RES_BUF_SIZE))
 *  return -1;
 * printf("%s\n", json_res);
 *
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_addr pointer to a kinotto_addr_t type.
 * @param dest pointer to buffer where the JSON output is stored.
 * @param n size of the output buffer.
 * @return 0 on success, -1 on failure
 */
int kinotto_json_sta_info(kinotto_wifi_sta_info_t *src, char *dest, int n);

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
 * kinotto_wifi_sta_detail_t scan_result[1024] = {0}; 
 * kinotto_wifi_sta = kinotto_wifi_sta_init("wlan0");
 * char json_res[JSON_RES_BUF_SIZE] = {0};
 *
 * if (!kinotto_wifi_sta)
 *  return -1;
 *
 * networks = kinotto_wifi_sta_scan_networks(kinotto_wifi_sta, scan_result,
 *              sizeof(scan_result) / sizeof(scan_result[0]));
 * if (-1 == networks)
 *  return -1;
 *
 * if (kinotto_json_sta_scan_result(scan_result, networks, json_res,
 *      JSON_RES_BUF_SIZE))
 *  return -1;
 * printf("%s\n", json_res);
 *
 * kinotto_wifi_sta_destroy(kinotto_wifi_sta);
 * @endcode
 *
 * @param kinotto_addr pointer to a kinotto_addr_t type.
 * @param scan_n size of the input buffer.
 * @param dest pointer to buffer where the JSON output is stored.
 * @param n size of the output buffer.
 * @return 0 on success, -1 on failure
 */
int kinotto_json_sta_scan_result(struct kinotto_wifi_sta_detail *scan_res,
				 int scan_n, char *dest, int n);

#ifdef __cplusplus
}
#endif

#endif
