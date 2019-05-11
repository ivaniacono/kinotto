#include "kinotto_ip_utils.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void child_redirect_stderr_to_null();
static int kinotto_ip_utils_has_ipv4(const char *ifname);

static void child_redirect_stderr_to_null()
{
	int std_fd = open("/dev/null", O_WRONLY);
	if (-1 == std_fd)
		_exit(1);
	dup2(std_fd, STDERR_FILENO);
	close(std_fd);
}

static int kinotto_ip_utils_has_ipv4(const char *ifname)
{
	kinotto_addr_t kinotto_addr = {0};

	if (kinotto_ip_utils_get_ipv4(ifname, &kinotto_addr))
		return -1;

	return 0;
}

int kinotto_ip_utils_ipv4_static(const char *ifname,
				 const kinotto_addr_t *kinotto_addr)
{
	struct ifreq ifr;
	struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	if (-1 == fd)
		goto error_fd;

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, ifname, KINOTTO_IFSIZE);

	if (inet_pton(AF_INET, kinotto_addr->ipv4_addr, &addr->sin_addr) != 1)
		goto error;

	if (ioctl(fd, SIOCSIFADDR, &ifr))
		goto error;

	if (inet_pton(AF_INET, kinotto_addr->ipv4_netmask, &addr->sin_addr) !=
	    1)
		goto error;

	if (ioctl(fd, SIOCSIFNETMASK, &ifr))
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

int kinotto_ip_utils_flush_ipv4(const char *ifname)
{
	kinotto_addr_t kinotto_addr = {"0.0.0.0", "0.0.0.0"};
	kinotto_ip_utils_ipv4_static(ifname, &kinotto_addr);

	return 0;
}

/* TODO: add router and DNS functionalities */

int kinotto_ip_utils_ipv4_dhcp(const char *ifname, int timeout)
{
#ifdef DHCLIENT
	int pid = 0;

	pid = fork();
	if (!pid) {
		child_redirect_stderr_to_null();

		execlp("dhclient", "dhclient", "-x", ifname, (char *)NULL);
		_exit(1);
	} else {
		wait(NULL);
	}

	pid = fork();
	if (!pid) {
		child_redirect_stderr_to_null();
		kinotto_ip_utils_flush_ipv4(ifname);
		execlp("dhclient", "dhclient", ifname, (char *)NULL,
		       (char *)NULL);
		_exit(1);
	} else {
		while (kinotto_ip_utils_has_ipv4(ifname)) {
			if (timeout > 0) {
				timeout--;
				sleep(1);
			} else {
				goto error;
			}
		}
	}

	return 0;
#endif

error:
	return -1;
}

int kinotto_ip_utils_get_ipv4(const char *ifname, kinotto_addr_t *kinotto_addr)
{
	struct ifreq ifr;
	struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

	memset(kinotto_addr->ipv4_addr, 0, KINOTTO_IPV4_STR_SIZE);
	memset(kinotto_addr->ipv4_netmask, 0, KINOTTO_IPV4_STR_SIZE);

	if (-1 == fd)
		goto error_fd;

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, ifname, KINOTTO_IFSIZE);

	if (ioctl(fd, SIOCGIFADDR, &ifr))
		goto error;

	strncpy(kinotto_addr->ipv4_addr, inet_ntoa(addr->sin_addr),
		KINOTTO_IPV4_STR_LEN);

	if (ioctl(fd, SIOCGIFNETMASK, &ifr))
		goto error;

	strncpy(kinotto_addr->ipv4_netmask, inet_ntoa(addr->sin_addr),
		KINOTTO_IPV4_STR_LEN);

	close(fd);

	return 0;

error:
	close(fd);
	return -1;

error_fd:
	return -1;
}

int kinotto_ip_utils_set_mac(const char *ifname,
			     const kinotto_addr_t *kinotto_addr)
{
	struct ifreq ifr;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
	int ret = 0;

	if (-1 == fd)
		goto error_fd;

	strncpy(ifr.ifr_name, ifname, KINOTTO_IFSIZE);
	if (strlen(kinotto_addr->mac_addr) != KINOTTO_MAC_STR_LEN)
		goto error;

	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	ret = sscanf(kinotto_addr->mac_addr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
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

int kinotto_ip_utils_rand_mac(const char *ifname)
{
	FILE *fd;
	unsigned char rand_mac[6] = {0};
	kinotto_addr_t kinotto_addr = {{0}, {0}, {0}};

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

	if (kinotto_ip_utils_set_mac(ifname, &kinotto_addr))
		goto error;

	fclose(fd);
	return 0;

error:
	fclose(fd);
	return -1;

error_fd:
	return -1;
}

int kinotto_ip_utils_get_mac(const char *ifname, kinotto_addr_t *kinotto_addr)
{
	struct ifreq ifr;
	int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);

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

// TODO: add IPv6 support
