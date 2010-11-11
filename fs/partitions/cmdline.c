#include <linux/init.h>
#include <linux/kernel.h>

#include "check.h"
#include "cmdline.h"

const char *partition_list = NULL;
static short set = 0;

#define STATE_NUM 0
#define STATE_START 1
#define STATE_SIZE 2


/*
 */
int
cmdline_partition(struct parsed_partitions *state, struct block_device *bdev)
{

        if (state->parts[0].size)
          return;

	int blocksize, res;
	char *ptr = partition_list;
	char *pstart = ptr;
	short parse_state = STATE_NUM;
	int conf[STATE_SIZE];


        res = 0;
	blocksize = bdev_hardsect_size(bdev);
	if (blocksize <= 0)
		goto out_exit;

        res = 1;

	do {

	ptr++;
	switch (*ptr) {
		case ':':
		case 0:
		case ',':
		conf[parse_state]=simple_strtol(pstart, NULL, 0);

		if(parse_state == STATE_SIZE) {
			parse_state = STATE_NUM;
			put_partition(state, conf[0], conf[1], conf[2]);
		} else {
			parse_state ++;
		}

		if(*ptr)
			pstart = ++ptr;

	    }
	} while (*ptr);

out_exit:
	return res;
}

static int __init partition_setup(char *options) {
    printk(KERN_INFO "partition setup %s\n", options);

    if (options && *options && !partition_list)
        partition_list = options;

    return 0;

}
__setup("cmdpart=", partition_setup);
