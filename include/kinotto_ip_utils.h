#ifndef __KINOTTO_IP_UTILS_H__
#define __KINOTTO_IP_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_types.h"

int kinotto_ip_utils_ipv4_static(const char *ifname,
					const kinotto_addr_t *kinotto_addr);
int kinotto_ip_utils_ipv4_dhcp(const char *ifname, int timeout);
int kinotto_ip_utils_flush_ipv4(const char *ifname);
int kinotto_ip_utils_get_ipv4(const char *ifname, kinotto_addr_t *kinotto_addr);

#ifdef __cplusplus
}
#endif

#endif
