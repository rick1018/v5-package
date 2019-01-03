#define TAG "wifi"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wifi_intf.h>
#include <pthread.h>

#define TEST_TIMES 1001

static pthread_t  app_scan_tid;
static int event = WIFIMG_NETWORK_DISCONNECTED;

static void wifi_event_handle(tWIFI_EVENT wifi_event, void *buf, int event_label)
{
    printf("event_label 0x%x\n", event_label);

    switch(wifi_event)
    {
        case WIFIMG_WIFI_ON_SUCCESS:
        {
            printf("WiFi on success!\n");
            event = WIFIMG_WIFI_ON_SUCCESS;
            break;
        }

        case WIFIMG_WIFI_ON_FAILED:
        {
            printf("WiFi on failed!\n");
            event = WIFIMG_WIFI_ON_FAILED;
            break;
        }

        case WIFIMG_WIFI_OFF_FAILED:
        {
            printf("wifi off failed!\n");
            event = WIFIMG_WIFI_OFF_FAILED;
            break;
        }

        case WIFIMG_WIFI_OFF_SUCCESS:
        {
            printf("wifi off success!\n");
            event = WIFIMG_WIFI_OFF_SUCCESS;
            break;
        }

        case WIFIMG_NETWORK_CONNECTED:
        {
            printf("WiFi connected ap!\n");
            event = WIFIMG_NETWORK_CONNECTED;
            break;
        }

        case WIFIMG_NETWORK_DISCONNECTED:
        {
            printf("WiFi disconnected!\n");
            event = WIFIMG_NETWORK_DISCONNECTED;
            break;
        }

        case WIFIMG_PASSWORD_FAILED:
        {
            printf("Password authentication failed!\n");
            event = WIFIMG_PASSWORD_FAILED;
            break;
        }

        case WIFIMG_CONNECT_TIMEOUT:
        {
            printf("Connected timeout!\n");
            event = WIFIMG_CONNECT_TIMEOUT;
            break;
        }

        case WIFIMG_NO_NETWORK_CONNECTING:
        {
            printf("It has no wifi auto connect when wifi on!\n");
            event = WIFIMG_NO_NETWORK_CONNECTING;
            break;
        }

        case WIFIMG_CMD_OR_PARAMS_ERROR:
        {
            printf("cmd or params error!\n");
            event = WIFIMG_CMD_OR_PARAMS_ERROR;
            break;
        }

        case WIFIMG_KEY_MGMT_NOT_SUPPORT:
        {
            printf("key mgmt is not supported!\n");
            event = WIFIMG_KEY_MGMT_NOT_SUPPORT;
            break;
        }

        case WIFIMG_OPT_NO_USE_EVENT:
        {
            printf("operation no use!\n");
            event = WIFIMG_OPT_NO_USE_EVENT;
            break;
        }

        case WIFIMG_NETWORK_NOT_EXIST:
        {
            printf("network not exist!\n");
            event = WIFIMG_NETWORK_NOT_EXIST;
            break;
        }

        case WIFIMG_DEV_BUSING_EVENT:
        {
            printf("wifi device busing!\n");
            event = WIFIMG_DEV_BUSING_EVENT;
            break;
        }

        default:
        {
            printf("Other event, no care!\n");
        }
    }
}

void *app_scan_task(void *args)
{
    const aw_wifi_interface_t *p_wifi = (aw_wifi_interface_t *)args;
    char scan_results[4096];
    int len = 0;
    int event_label = 0;

    while(1){
        event_label++;
        p_wifi->start_scan(event_label);
        len = 4096;
        p_wifi->get_scan_results(scan_results, &len);
    }
}

/*
 *argc[1]   ap ssid
 *argc[2]   ap passwd
 *agrc[3]   [test times]
 *see the test result in file:/etc/test_result
*/
int main(int argv, char *argc[]){
    int i = 0, ret = 0, len = 0;
    int times = 0, event_label = 0;;
    char ssid[256] = {0}, scan_results[4096] = {0};
    int  wifi_state = WIFIMG_WIFI_DISABLED;
    const aw_wifi_interface_t *p_wifi_interface = NULL;
	int success_times = 0, fail_times = 0, timeout_times = 0;
	char prt_buf[256] = {0};
	int ttest_times = 0;

	if(NULL == argc[1]){
		printf("Usage:wifi_long_test <ssid> <psk> [test_times]!\n");
		printf("default test times: %d\n",TEST_TIMES);
		return -1;
	}
	if(NULL == argc[3])
		ttest_times = TEST_TIMES;
	else
		ttest_times = atoi(argc[3]);

    printf("\n*********************************\n");
    printf("***Start wifi long test!***\n");
    printf("*********************************\n");

	printf("Test times: %d\n",ttest_times);

	sleep(2);

    event_label = rand();
    p_wifi_interface = aw_wifi_on(wifi_event_handle, event_label);
    if(p_wifi_interface == NULL){
        printf("wifi on failed event 0x%x\n", event);
        return -1;
    }

    while(aw_wifi_get_wifi_state() == WIFIMG_WIFI_BUSING){
        printf("wifi state busing,waiting\n");
        usleep(2000000);
    }

    //pthread_create(&app_scan_tid, NULL, &app_scan_task,(void *)p_wifi_interface);
	for(i=0;i<ttest_times;i++){
		event_label++;
		p_wifi_interface->connect_ap(argc[1], argc[2], event_label);

	while(aw_wifi_get_wifi_state() == WIFIMG_WIFI_BUSING){
		printf("wifi state busing,waiting\n");
		usleep(2000000);
	}

		if(event == WIFIMG_NETWORK_CONNECTED || event == WIFIMG_CONNECT_TIMEOUT)
		{
			success_times++;
			if(event == WIFIMG_CONNECT_TIMEOUT)
				timeout_times++;
		}
		else
		{
			fail_times++;
		}
		printf("Test Times: %d\nSuccess Times: %d\nFailed Times: %d\n",i+1,success_times,fail_times);

		event_label++;
		p_wifi_interface->disconnect_ap(event_label);

	while(aw_wifi_get_wifi_state() == WIFIMG_WIFI_BUSING){
		printf("wifi state busing,waiting\n");
		usleep(2000000);
	}
	}

	sprintf(prt_buf,"echo \"Test Times:%d, Success Times:%d(including get IP timeout times:%d), Failed Times:%d\" > /etc/test_results",i,success_times,timeout_times,fail_times);
	system(prt_buf);
	if(success_times == ttest_times)
	{
	sprintf(prt_buf,"echo Congratulations! >> /etc/test_results");
	system(prt_buf);
	}

    printf("******************************\n");
    printf("Wifi connect ap test: Success!\n");
    printf("******************************\n");

    return 0;
}