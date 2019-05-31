#include "kinotto_if.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

int kinotto_if_get_ifaces(kinotto_info_t *dest, int n)
{
	struct if_nameindex *if_ni, *i;
	int ret = 0;

	if (!dest)
		goto error;

	if_ni = if_nameindex();
	if (!if_ni) {
		goto error;
	}

	i = if_ni;
	while (i->if_index && i->if_name && i->if_index < n) {
		snprintf(dest[i->if_index - 1].ifname, KINOTTO_IFSIZE,
			 "%s", i->if_name);
		i++;
		ret++;
	}

	if_freenameindex(if_ni);

	return ret;

error:
	return -1;
}

int kinotto_if_set_mac(const char *ifname,
			     const kinotto_addr_t *mac)
{
	struct ifreq ifr;
	int fd;
	int ret = 0;

	if (!strlen(ifname) || !mac)
		goto error;

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (-1 == fd)
		goto error_fd;

	strncpy(ifr.ifr_name, ifname, KINOTTO_IFSIZE);
	if (strlen(mac->mac_addr) != KINOTTO_MAC_STR_LEN)
		goto error;

	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	ret = sscanf(mac->mac_addr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
		     &ifr.ifr_hwaddr.sa_data[0], &ifr.ifr_hwaddr.sa_data[1],
		     &ifr.ifr_hwaddr.sa_data[2], &ifr.ifr_hwaddr.sa_data[3],
		     &ifr.ifr_hwaddr.sa_data[4], &ifr.ifr_hwaddr.sa_data[5]);
	if (ret != 6)
		goto error;

	ifr.ifr_flags &= ~IFF_UP;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr))
		goto error;

	ifr.ifr_flags = IFF_UP;
	if (ioctl(fd, SIOCSIFHWADDR, &ifr))
		goto error;

	ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
	if (ioctl(fd, SIOCSIFFLAGS, &ifr))
		goto error;

	close(fd);
	return 0;

error:
	close(fd);
	return -1;

error_fd:
	return -1;
}

int kinotto_if_rand_mac(const char *ifname)
{
	FILE *fd;
	unsigned char rand_mac[6] = {0};
	kinotto_addr_t kinotto_addr = {{0}, {0}, {0}};

	if (!strlen(ifname))
		goto error;

	fd = fopen("/dev/urandom", "rb");
	if (!fd)
		goto error_fd;

	if (!fread(rand_mac, 1, sizeof(rand_mac), fd))
		goto error;

	rand_mac[0] &= 0xFE;
	rand_mac[0] |= 0x02;

	snprintf(kinotto_addr.mac_addr, KINOTTO_MAC_STR_SIZE,
		 "%02x:%02x:%02x:%02x:%02x:%02x", rand_mac[0], rand_mac[1],
		 rand_mac[2], rand_mac[3], rand_mac[4], rand_mac[5]);

	if (kinotto_if_set_mac(ifname, &kinotto_addr))
		goto error;

	fclose(fd);
	return 0;

error:
	fclose(fd);
	return -1;

error_fd:
	return -1;
}

int kinotto_if_get_mac(const char *ifname, kinotto_addr_t *kinotto_addr)
{
	struct ifreq ifr;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (!strlen(ifname) || !kinotto_addr)
		goto error;

	memset(kinotto_addr->mac_addr, 0, KINOTTO_MAC_STR_SIZE);

	if (-1 == fd)
		goto error_fd;

	ifr.ifr_addr.sa_family = ARPHRD_ETHER;

	strncpy(ifr.ifr_name, ifname, KINOTTO_IFSIZE);

	if (ioctl(fd, SIOCGIFHWADDR, &ifr))
		goto error;

	snprintf(kinotto_addr->mac_addr, KINOTTO_MAC_STR_SIZE,
		 "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		 ifr.ifr_hwaddr.sa_data[0], ifr.ifr_hwaddr.sa_data[1],
		 ifr.ifr_hwaddr.sa_data[2], ifr.ifr_hwaddr.sa_data[3],
		 ifr.ifr_hwaddr.sa_data[4], ifr.ifr_hwaddr.sa_data[5]);

	close(fd);

	return 0;

error:
	close(fd);
	return -1;

error_fd:
	return -1;
}
