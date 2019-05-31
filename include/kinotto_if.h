/**
 * @file kinotto_if.h
 * @author Ivan Iacono
 * @brief Kinotto interface utils.
 *
 * This header provides prototypes for interface utilities.
 */

#ifndef __KINOTTO_IF_H__
#define __KINOTTO_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_types.h"

/**
 * @brief Assign a MAC address.
 *
 * Assign a MAC address to an interface.
 *
 * @code
 * kinotto_addr_t addr;
 *
 * memset(addr.mac_addr, 0, KINOTTO_MAC_STR_SIZE);
 * strncpy(addr.mac_addr, "A0:B0:C0:D0:E0:F0", KINOTTO_MAC_STR_LEN);
 *
 * if (kinotto_if_set_mac("wlan0", &addr))
 * 	return -1;
 * @endcode
 *
 * @param ifname interface to use.
 * @param mac pointer to a kinotto_addr_t containing the MAC address;
 * @return 0 on success, -1 on failure
 */
int kinotto_if_set_mac(const char *ifname,
			     const kinotto_addr_t *mac);

/**
 * @brief Get MAC address.
 *
 * Get MAC address of a specified interface.
 *
 * @code
 * kinotto_addr_t kinotto_addr = {0};
 *
 * if (kinotto_if_get_mac("wlan0", &kinotto_addr)) {
 * 	return -1;
 * }
 * @endcode
 *
 * @param ifname interface to use.
 * @param dest pointer to a kinotto_addr_t.
 * @return 0 on success, -1 on error.
 */
int kinotto_if_get_mac(const char *ifname, kinotto_addr_t *dest);

/**
 * @brief Assign a random MAC address
 *
 * Assign a random MAC address to an interface.
 *
 * @code
 * kinotto_if_rand_mac("wlan0");
 * @endcode
 *
 * @param ifname interface to use.
 * @return 0 on success, -1 on error.
 */
int kinotto_if_rand_mac(const char *ifname);

/**
 * @brief Get all interfaces available
 *
 * List all the system interfaces available.
 *
 * @code
 * #define MAX_IFACES 12
 * ...
 * kinotto_info_t info[MAX_IFACES] = {0};
 *
 * kinotto_if_get_ifaces(info, MAX_IFACES);
 * @endcode
 *
 * @param dest pointer to a list of kinotto_info_t.
 * @param n number of kinotto_info_t elements.
 * @return numer of interfaces, -1 on error.
 */
int kinotto_if_get_ifaces(kinotto_info_t *dest, int n);

#ifdef __cplusplus
}
#endif

#endif
