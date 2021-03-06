#include <kinotto/kinotto_if.h>
#include <kinotto/kinotto_net.h>
#include <kinotto/kinotto_json.h>
#include <kinotto/kinotto_wifi_sta.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define JSON_RES_BUF_SIZE 144 * 1024
#define DEFAULT_WIFI_CLI_IF "wlan0"
#define DHCP_TIMEOUT_S 30
#define WIFI_STA_CONNECT_TIMEOUT_S 10
#define MAX_IFACES 12

enum cmd {
	IP_ONLY,
	IP_INFO,
	MAC_ONLY,
	WIFI_SCAN,
	WIFI_CLI,
	WIFI_INFO,
	WIFI_DISCONNECT
};

struct kinottocli_args {
	int cmd;
	int json_output;
	int dhcp;
	int save;
	int quiet_psk;
	int flush;
	int rand_mac;
	char ifname[KINOTTO_IFSIZE];
	kinotto_addr_t addr;
	kinotto_wifi_sta_connect_t sta_connect;
};

struct kinottocli_args cli_args = {
    0, 0, 1, 0, 0, 0, 0, {0}, {{0}, {0}, {0}}, {{0}, {0}, 0, 0}};

static void print_help(const char *name)
{
	fprintf(stderr, "   _                                                       \n");
	fprintf(stderr, "  !.!     _   ___             _   _             _ _        \n");
	fprintf(stderr, "  : :    | | / (_)           | | | |           | (_)       \n");
	fprintf(stderr, " :   :   | |/ / _ _ __   ___ | |_| |_ ___   ___| |_        \n");
	fprintf(stderr, "!_____!  |    \\| | '_ \\ / _ \\| __| __/ _ \\ / __| | |   \n");
	fprintf(stderr, "!__K__!  | |\\  \\ | | | | (_) | |_| || (_) | (__| | |     \n");
	fprintf(stderr, ":     :  \\_| \\_/_|_| |_|\\___/ \\__|\\__\\___/ \\___|_|_|\n");
	fprintf(stderr, " '''''                                     \n");
	fprintf(stderr, "Usage: %s -i INTERFACE COMMAND [OPTION]...\n", name);
	fprintf(stderr, "\n");
	fprintf(stderr, " ip                      assign IP address\n");
	fprintf(stderr, " mac 'A0:B0:C0:D0:E0:F0' assign MAC address\n");
	fprintf(stderr, " scan                    scan networks\n");
	fprintf(stderr, " info                    get current interface state "
			"(default command)\n");
	fprintf(stderr, " connect 'SSID' 'PSK'    connect to a network\n");
	fprintf(stderr, " disconnect              disconnect from network\n");
	fprintf(stderr,
		" sta_info                get current Wi-Fi interface state\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options summary:\n\n");
	fprintf(stderr, " GENERIC\n");
	fprintf(stderr, "   -h       print this help\n");
	fprintf(stderr, "   -j       output as JSON\n");
	fprintf(stderr, "\n IP ADDRESS\n");
	fprintf(stderr, "   -a       DHCP (default)\n");
	fprintf(stderr, "   -4       IPv4 address\n");
	fprintf(stderr, "   -n       IPv4 netmask\n");
	fprintf(stderr, "   -f       IPv4 flush\n");
	fprintf(stderr, "\n MAC ADDRESS\n");
	fprintf(stderr, "   -r       assign a random MAC Address\n");
	fprintf(stderr, "\n WIFI CONNECTION\n");
	fprintf(stderr, "   -s       save network config on success\n");
	fprintf(stderr, "   -q       get PSK from prompt\n");
	fprintf(stderr, "\n");
}

static void print_scan_result(kinotto_wifi_sta_detail_t *scan_result,
			      size_t size)
{
	int i = 0;
	const int n = 105;
	const char header_row[] = "║ %-17s\t %-32s\t %15s\t %5s\t %9s ║\n";
	const char entry_row[] = "║ %-17s\t %-32s\t %15s\t %5d\t %9d ║\n";

	printf("╔");
	for (i = 0; i <= n; i++)
		printf("═");
	printf("╗\n");

	printf(header_row, "BSSID", "SSID", "SECURITY", "LEVEL", "FREQUENCY");

	printf("╟");
	for (i = 0; i <= n; i++)
		printf("─");
	printf("╢\n");

	for (i = 0; i < size; i++) {
		printf(entry_row, scan_result[i].bssid, scan_result[i].ssid,
		       scan_result[i].security, scan_result[i].level,
		       scan_result[i].frequency);
	}

	printf("╚");
	for (i = 0; i <= n; i++)
		printf("═");
	printf("╝\n");
}

static void print_scan_result_json(kinotto_wifi_sta_detail_t *scan_result,
				   int networks)
{
	char json_res[JSON_RES_BUF_SIZE] = {0};

	if (kinotto_json_sta_scan_result(scan_result, networks, json_res,
					      JSON_RES_BUF_SIZE))
		return;

	printf("%s\n", json_res);
}

static void print_sta_info_json(kinotto_wifi_sta_info_t *sta_info)
{
	char json_res[JSON_RES_BUF_SIZE] = {0};

	if (kinotto_json_sta_info(sta_info, json_res, JSON_RES_BUF_SIZE))
		return;

	printf("%s\n", json_res);
}

static void print_ip_info_json(kinotto_info_t *kinotto_info, int n)
{
	char json[JSON_RES_BUF_SIZE] = {0};

	if (n > 1) {
		kinotto_json_ifaces_list(kinotto_info, n, json, JSON_RES_BUF_SIZE);
	} else {
		kinotto_json_ip_info(kinotto_info, json, JSON_RES_BUF_SIZE);
	}

	printf("%s\n", json);
}

static void print_ip_info(kinotto_info_t *kinotto_info, int n)
{
	int i = 0;

	for (i = 0; i < n; i++) {
		printf("Interface: %s\n", kinotto_info[i].ifname);
		printf("MAC Addr: %s\n", kinotto_info[i].addr.mac_addr);
		printf("IPv4: %s\n", kinotto_info[i].addr.ipv4_addr);
		printf("Netmask: %s\n", kinotto_info[i].addr.ipv4_netmask);
		printf("\n");
	}
}

static void print_wifi_sta_info(kinotto_wifi_sta_info_t *kinotto_wifi_sta_info)
{
	if (KINOTTO_WIFI_STA_CONNECTED == kinotto_wifi_sta_info->state) {
		printf("Status: CONNECTED\n");
		printf("BSSID: %s\n", kinotto_wifi_sta_info->sta.bssid);
		printf("SSID: %s\n", kinotto_wifi_sta_info->sta.ssid);
		printf("Security: %s\n", kinotto_wifi_sta_info->sta.security);
		printf("Frequency: %d\n", kinotto_wifi_sta_info->sta.frequency);
	} else {
		printf("Status: DISCONNECTED\n");
	}
}

static int assign_ipv4_dhcp(const char *ifname)
{
	if (kinotto_net_ipv4_dhcp(ifname, DHCP_TIMEOUT_S)) {
		return -1;
	}

	return 0;
}

static int parse_args(int argc, char *argv[])
{
	int c = 0;
	char *qpsk;

	while ((c = getopt(argc, argv, "i:hsjaqfr4:n:")) && (c != -1)) {
		switch (c) {
		case 'h':
			goto help;
		case 'i':
			memset(cli_args.ifname, 0, KINOTTO_IFSIZE);
			if (strlen(optarg) > KINOTTO_IFSIZE)
				goto error;
			strncpy(cli_args.ifname, optarg, KINOTTO_IFSIZE);
			break;
		case 's':
			cli_args.save = 1;
		case 'j':
			cli_args.json_output = 1;
			break;
		case 'a':
			cli_args.dhcp = 1;
			break;
		case '4':
			if (strlen(optarg) > KINOTTO_IPV4_STR_LEN)
				goto error;
			cli_args.dhcp = 0;
			memset(cli_args.addr.ipv4_addr, 0,
			       KINOTTO_IPV4_STR_SIZE);
			memset(cli_args.addr.ipv4_netmask, 0,
			       KINOTTO_IPV4_STR_SIZE);
			strncpy(cli_args.addr.ipv4_addr, optarg,
				KINOTTO_IPV4_STR_LEN);
			strncpy(cli_args.addr.ipv4_netmask, "255.255.255.0",
				KINOTTO_IPV4_STR_LEN);
			break;
		case 'n':
			if (strlen(optarg) > KINOTTO_IPV4_STR_LEN)
				goto error;
			memset(cli_args.addr.ipv4_netmask, 0,
			       KINOTTO_IPV4_STR_SIZE);
			strncpy(cli_args.addr.ipv4_netmask, optarg,
				KINOTTO_IPV4_STR_LEN);
			break;
		case 'r':
			cli_args.rand_mac = 1;
		case 'f':
			cli_args.flush = 1;
			break;
		case 'q':
			cli_args.quiet_psk = 1;
			break;
		default:
			goto error;
		}
	}

	if (optind < argc) {
		if (!strncmp(argv[optind], "scan", 4)) {
			cli_args.cmd = WIFI_SCAN;
		} else if (!strncmp(argv[optind], "connect", 7)) {
			cli_args.cmd = WIFI_CLI;
			if ((optind + 1) < argc) {
				cli_args.sta_connect.remove_all = 1;
				strncpy(cli_args.sta_connect.ssid,
					argv[optind + 1],
					KINOTTO_WIFI_STA_SSID_LEN);
			}
			if ((optind + 2) < argc) {
				strncpy(cli_args.sta_connect.psk,
					argv[optind + 2],
					KINOTTO_WIFI_STA_PSK_LEN);
			} else {
				if (cli_args.quiet_psk) {
					qpsk = getpass("PSK: ");
					if (qpsk) {
						strncpy(
						    cli_args.sta_connect.psk,
						    qpsk,
						    KINOTTO_WIFI_STA_PSK_LEN);
					} else {
						goto error;
					}
				} else {
					memset(cli_args.sta_connect.psk, '\0',
					       KINOTTO_WIFI_STA_PSK_LEN);
				}
			}
		} else if (!strncmp(argv[optind], "disconnect", 10)) {
			cli_args.cmd = WIFI_DISCONNECT;
		} else if (!strncmp(argv[optind], "info", 4)) {
			cli_args.cmd = IP_INFO;
		} else if (!strncmp(argv[optind], "sta_info", 8)) {
			cli_args.cmd = WIFI_INFO;
		} else if (!strncmp(argv[optind], "mac", 3)) {
			cli_args.cmd = MAC_ONLY;
			if ((optind + 1) < argc) {
				strncpy(cli_args.addr.mac_addr,
					argv[optind + 1], KINOTTO_MAC_STR_LEN);
			}
		} else if (!strncmp(argv[optind], "ip", 2)) {
			cli_args.cmd = IP_ONLY;
		} else {
			cli_args.cmd = IP_INFO;
		}
	} else {
		cli_args.cmd = IP_INFO;
	}

	return 0;

error:
	return -1;

help:
	print_help(argv[0]);
	return -1;
}

static int exec_wifi_scan()
{
	int networks = 0;
	kinotto_wifi_sta_t *kinotto_wifi_sta;
	kinotto_wifi_sta_detail_t scan_result[1024];

	kinotto_wifi_sta = kinotto_wifi_sta_init(cli_args.ifname);
	if (!kinotto_wifi_sta)
		return -1;

	networks = kinotto_wifi_sta_scan_networks(kinotto_wifi_sta, scan_result,
						  sizeof(scan_result) /
						      sizeof(scan_result[0]));

	if (-1 == networks)
		goto error;

	if (cli_args.json_output) {
		print_scan_result_json(scan_result, networks);
	} else {
		print_scan_result(scan_result, networks);
	}

	kinotto_wifi_sta_destroy(kinotto_wifi_sta);

	return 0;

error:
	kinotto_wifi_sta_destroy(kinotto_wifi_sta);
	return -1;
}

static int get_ip_info(kinotto_info_t *kinotto_info)
{
	kinotto_addr_t *kinotto_addr = &kinotto_info->addr;

	if (kinotto_if_get_mac(kinotto_info->ifname, kinotto_addr)) {
		strncpy(kinotto_addr->mac_addr, "NOT_ASSIGNED",
			KINOTTO_MAC_STR_LEN);
	}

	if (kinotto_net_get_ipv4(kinotto_info->ifname, kinotto_addr)) {
		strncpy(kinotto_addr->ipv4_addr, "NOT_ASSIGNED",
			KINOTTO_IPV4_STR_LEN);
		strncpy(kinotto_addr->ipv4_netmask, "NOT_ASSIGNED",
			KINOTTO_IPV4_STR_LEN);
	}

	return 0;
}

static int exec_ip_info()
{
	int i = 0;
	int ifaces = 0;
	kinotto_info_t info[MAX_IFACES] = {0};

	if (strlen(cli_args.ifname)) {
		strncpy(info[i].ifname, cli_args.ifname, KINOTTO_IFSIZE);
		ifaces = 1;
	} else {
		ifaces = kinotto_if_get_ifaces(info, MAX_IFACES);
	}

	for (i = 0; i < ifaces; i++) {
		get_ip_info(&info[i]);
	}

	if (cli_args.json_output) {
		print_ip_info_json(info, ifaces);
	} else {
		print_ip_info(info, ifaces);
	}

	return 0;
}

static int exec_wifi_info()
{
	kinotto_wifi_sta_t *kinotto_wifi_sta;
	kinotto_wifi_sta_info_t kinotto_wifi_sta_info;

	kinotto_wifi_sta = kinotto_wifi_sta_init(cli_args.ifname);
	if (!kinotto_wifi_sta)
		return -1;

	kinotto_wifi_sta_get_info(kinotto_wifi_sta, &kinotto_wifi_sta_info);

	if (cli_args.json_output) {
		print_sta_info_json(&kinotto_wifi_sta_info);
	} else {
		print_wifi_sta_info(&kinotto_wifi_sta_info);
	}

	kinotto_wifi_sta_destroy(kinotto_wifi_sta);

	return 0;
}

static int exec_ip_only()
{
	if (cli_args.flush) {
		if (kinotto_net_flush_ipv4(cli_args.ifname))
			goto error;
	} else if (cli_args.dhcp) {
		printf("Assigning DHCP address...");
		if (assign_ipv4_dhcp(cli_args.ifname))
			goto error;
	} else {
		if (kinotto_net_set_ipv4(cli_args.ifname,
						 &cli_args.addr))
			goto error;
	}

	printf("OK\n");
	return 0;

error:
	printf("failed!\n");
	return -1;
}

static int exec_mac_only()
{
	if (cli_args.rand_mac) {
		if (kinotto_if_rand_mac(cli_args.ifname))
			goto error;
	} else {
		if (kinotto_if_set_mac(cli_args.ifname, &cli_args.addr))
			goto error;
	}

	printf("OK\n");
	return 0;

error:
	printf("failed\n");
	return -1;
}

static int exec_wifi_client()
{
	int rc = 0;
	kinotto_wifi_sta_t *kinotto_wifi_sta;
	kinotto_wifi_sta_info_t kinotto_wifi_sta_info;

	kinotto_wifi_sta = kinotto_wifi_sta_init(cli_args.ifname);
	if (!kinotto_wifi_sta)
		return -1;

	cli_args.sta_connect.timeout = WIFI_STA_CONNECT_TIMEOUT_S;

	printf("Connecting to %s...", cli_args.sta_connect.ssid);
	rc = kinotto_wifi_sta_connect_network(
	    kinotto_wifi_sta, &kinotto_wifi_sta_info, &cli_args.sta_connect);

	if (rc) {
		printf("failed\n");
		goto error;
	}
	printf("OK\n");

	if (exec_ip_only()) {
		goto error;
	}

	if (cli_args.save) {
		printf("Saving configuration...");
		if (kinotto_wifi_sta_save_config(kinotto_wifi_sta)) {
			printf("failed\n");
			goto error;
		}
		printf("OK\n");
	}

	kinotto_wifi_sta_destroy(kinotto_wifi_sta);

	return 0;

error:
	kinotto_wifi_sta_destroy(kinotto_wifi_sta);
	return -1;
}

static int exec_wifi_disconnect()
{
	int rc = 0;
	kinotto_wifi_sta_t *kinotto_wifi_sta;
	kinotto_wifi_sta_info_t kinotto_wifi_sta_info;
	kinotto_wifi_sta = kinotto_wifi_sta_init(cli_args.ifname);
	if (!kinotto_wifi_sta)
		return -1;

	rc = kinotto_wifi_sta_disconnect_network(kinotto_wifi_sta,
						 &kinotto_wifi_sta_info);

	if (rc) {
		printf("failed\n");
	}
	printf("OK\n");

	kinotto_net_flush_ipv4(cli_args.ifname);

	kinotto_wifi_sta_destroy(kinotto_wifi_sta);

	return 0;
}

static int exec_cmd()
{
	int ret = 0;

	switch (cli_args.cmd) {
	case IP_ONLY:
		ret = exec_ip_only();
		break;
	case IP_INFO:
		ret = exec_ip_info();
		break;
	case WIFI_SCAN:
		ret = exec_wifi_scan();
		break;
	case WIFI_CLI:
		ret = exec_wifi_client();
		break;
	case WIFI_DISCONNECT:
		ret = exec_wifi_disconnect();
		break;
	case WIFI_INFO:
		ret = exec_wifi_info();
		break;
	case MAC_ONLY:
		ret = exec_mac_only();
		break;
	default:
		return -1;
	}

	return ret;
}

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	if (parse_args(argc, argv)) {
		goto args_error;
	}

	return exec_cmd();

args_error:
	return 1;
}
