#ifndef __KINOTTO_TYPES_H__
#define __KINOTTO_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <arpa/inet.h>
#include <linux/if.h>

#define KINOTTO_IFSIZE IFNAMSIZ
#define KINOTTO_IPV4_STR_LEN 15
#define KINOTTO_IPV4_STR_SIZE (KINOTTO_IPV4_STR_LEN + 1)

typedef struct kinotto_addr {
	char ipv4_addr[KINOTTO_IPV4_STR_SIZE];
	char ipv4_netmask[KINOTTO_IPV4_STR_SIZE];
} kinotto_addr_t;

typedef struct kinotto_info {
	char ifname[KINOTTO_IFSIZE];
	kinotto_addr_t addr;
} kinotto_info_t;

#ifdef __cplusplus
}
#endif

#endif
