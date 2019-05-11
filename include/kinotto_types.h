/**
 * @file kinotto_types.h
 * @author Ivan Iacono
 * @brief Kinotto types definitions.
 *
 * This header provides type definitions to use with kinotto functions.
 */

#ifndef __KINOTTO_TYPES_H__
#define __KINOTTO_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <linux/if.h>

/**
 * Interface name length
 */
#define KINOTTO_IFSIZE IFNAMSIZ

/**
 * IPv4 string length
 */
#define KINOTTO_IPV4_STR_LEN 15

/**
 * IPv4 string vector size
 */
#define KINOTTO_IPV4_STR_SIZE (KINOTTO_IPV4_STR_LEN + 1)

/**
 * MAC string length
 */
#define KINOTTO_MAC_STR_LEN 17

/**
 * MAC string vector size
 */
#define KINOTTO_MAC_STR_SIZE (KINOTTO_MAC_STR_LEN + 1)

/**
 * Structure to contain IP addresses
 */
typedef struct kinotto_addr {
	/*@{*/
	char ipv4_addr[KINOTTO_IPV4_STR_SIZE]; /**< IPv4 string */
	char ipv4_netmask[KINOTTO_IPV4_STR_SIZE]; /**< IPv4 netmask string */
	char mac_addr[KINOTTO_MAC_STR_SIZE]; /**< MAC string */
	/*@}*/
} kinotto_addr_t;

/**
 * Structure to contain interface information
 */
typedef struct kinotto_info {
	/*@{*/
	char ifname[KINOTTO_IFSIZE]; /**< network interface name */
	kinotto_addr_t addr; /**< address information */
	/*@}*/
} kinotto_info_t;

#ifdef __cplusplus
}
#endif

#endif
