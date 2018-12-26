#include "kinotto_ip_utils.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/if.h>
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
	if (ioctl(fd, SIOCGIFFLAGS, &ifr))
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

	memset(kinotto_addr, 0, sizeof(kinotto_addr_t));

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

// TODO: add IPv6 support
