#ifndef __KINOTTO_JSON_UTILS_H__
#define __KINOTTO_JSON_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "kinotto_types.h"
#include "kinotto_wifi_sta.h"

int kinotto_ip_utils_ip_info_json(kinotto_addr_t *kinotto_addr, char *buf,
				  int buf_size);
int kinotto_wifi_sta_info_json(kinotto_wifi_sta_info_t *kinotto_wifi_sta_info,
			       char *buf, int buf_size);
int kinotto_wifi_sta_scan_result_json(
    struct kinotto_wifi_sta_scan_result *scan_res, int scan_size, char *buf,
    int buf_size);



#ifdef __cplusplus
}
#endif

#endif
