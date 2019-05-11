/**
 * @file kinotto_ip_utils.h
 * @author Ivan Iacono
 * @brief Kinotto IP utils.
 *
 * This header provides prototypes for IP utilities.
 */

#ifndef __KINOTTO_IP_UTILS_H__
#define __KINOTTO_IP_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_types.h"

/**
 * @brief Assign a static IP address.
 *
 * Assign a static IP address to an interface.
 *
 * @code
 * kinotto_addr_t addr;
 *
 * memset(addr.ipv4_addr, 0, KINOTTO_IPV4_STR_SIZE);
 * memset(addr.ipv4_netmask, 0, KINOTTO_IPV4_STR_SIZE);
 * strncpy(addr.ipv4_addr, "192.168.1.50", KINOTTO_IPV4_STR_LEN);
 * strncpy(addr.ipv4_netmask, "255.255.255.0", KINOTTO_IPV4_STR_LEN);
 *
 * if (kinotto_ip_utils_ipv4_static("wlan0", &addr);
 * 	return -1;
 * @endcode
 *
 * @param ifname interface to use.
 * @param kinotto_addr pointer to a kinotto_addr_t;
 * @return 0 on success, -1 on failure
 */
int kinotto_ip_utils_ipv4_static(const char *ifname,
				 const kinotto_addr_t *kinotto_addr);

/**
 * @brief Assign an IP address via DHCP.
 *
 * Assign an IP address to an interface via DHCP.
 *
 * @code
 * if (kinotto_ip_utils_ipv4_dhcp("wlan0", 30))
 * 	return -1;
 * @endcode
 *
 * @param ifname interface to use.
 * @param timeout DHCP timeout to use.
 * @return 0 on success, -1 on failure
 */
int kinotto_ip_utils_ipv4_dhcp(const char *ifname, int timeout);

/**
 * @brief Flush interface.
 *
 * Flush IP of a specified interface.
 *
 * @code
 * kinotto_ip_utils_flush_ipv4("wlan0");
 * @endcode
 *
 * @param ifname interface to use.
 * @return 0 on success.
 */
int kinotto_ip_utils_flush_ipv4(const char *ifname);

/**
 * @brief Get IPv4 information.
 *
 * Get IPv4 information for a specified interface.
 *
 * @code
 * kinotto_addr_t kinotto_addr = {0};
 *
 * if (kinotto_ip_utils_get_ipv4("wlan0", &kinotto_addr)) {
 * 	return -1;
 * }
 * @endcode
 *
 * @param ifname interface to use.
 * @param kinotto_addr pointer to a kinotto_addr_t.
 * @return 0 on success, -1 on error.
 */
int kinotto_ip_utils_get_ipv4(const char *ifname, kinotto_addr_t *kinotto_addr);

/**
 * @brief Assign a MAC address.
 *
 * Assign a MAC address to an interface.
 *
 * @code
 * kinotto_addr_t kinotto_addr;
 *
 * memset(addr.mac_addr, 0, KINOTTO_MAC_STR_SIZE);
 * strncpy(addr.mac_addr, "DE:AD:BE:EF:DE:AD", KINOTTO_MAC_STR_LEN);
 *
 * if (kinotto_ip_utils_set_mac("wlan0", &kinotto_addr))
 * 	return -1;
 * @endcode
 *
 * @param ifname interface to use.
 * @param kinotto_addr pointer to a kinotto_addr_t;
 * @return 0 on success, -1 on failure
 */
int kinotto_ip_utils_set_mac(const char *ifname,
			     const kinotto_addr_t *kinotto_addr);

/**
 * @brief Get MAC address.
 *
 * Get MAC address of a specified interface.
 *
 * @code
 * kinotto_addr_t kinotto_addr = {0};
 *
 * if (kinotto_ip_utils_get_mac("wlan0", &kinotto_addr)) {
 * 	return -1;
 * }
 * @endcode
 *
 * @param ifname interface to use.
 * @param kinotto_addr pointer to a kinotto_addr_t.
 * @return 0 on success, -1 on error.
 */
int kinotto_ip_utils_get_mac(const char *ifname, kinotto_addr_t *kinotto_addr);

/**
 * @brief Assign a random MAC address
 *
 * Assign a random MAC address to an interface.
 *
 * @code
 * kinotto_ip_utils_rand_mac("wlan0");
 * @endcode
 *
 * @param ifname interface to use.
 * @return 0 on success, -1 on error.
 */
int kinotto_ip_utils_rand_mac(const char *ifname);

#ifdef __cplusplus
}
#endif

#endif
