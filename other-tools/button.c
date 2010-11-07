#define __KERNEL_STRICT_NAMES
#define _GNU_SOURCE
#include <linux/input.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "nvodm_ec.h"

int main(int argc, char **argv) {
	int fd=open("/dev/ec_odm", O_RDWR);

	ec_odm_event_params params;

	params.operation=WAKEUP_EVENT_CONFIG;
	params.sub_operation=ENABLE_POWER_BUTTON_WAKEUP_EVENT;
	printf("ioctl=%d\n", ioctl(fd, EVENT_CONFIG, &params));

	sleep(1);
	params.sub_operation=ENABLE_HOMEKEY_WAKEUP_EVENT;
	printf("ioctl=%d\n", ioctl(fd, EVENT_CONFIG, &params));
	sleep(1);

	params.sub_operation=ENABLE_LID_SWITCH_WAKEUP_EVENT;
	printf("ioctl=%d\n", ioctl(fd, EVENT_CONFIG, &params));

	sleep(1);

	close(fd);
	return 0;
}
