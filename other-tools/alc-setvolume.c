#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int fd;
unsigned short read_i2c(unsigned char reg) {
	int err;
	struct i2c_rdwr_ioctl_data dat;
	unsigned addr=0x3C>>1;
	unsigned short data;
	struct i2c_msg msgs[]= {
		{
			.addr = addr,
			.flags = 0,
			.len = 1,
			.buf = &reg,
		},
		{
			.addr = addr,
			.flags = I2C_M_RD,
			.len = 2,
			.buf =(unsigned char*) &data,
		}
	};
	dat.msgs=msgs;
	dat.nmsgs=2;
	
	err=ioctl(fd, I2C_RDWR, &dat);
	if(err==-1) {
		perror("I2C_RDWR");
		return 0;
	}
	//Make it upside down
	data=data<<8 | (data&0xff00)>>8;
	return data;
}

void write_i2c(unsigned char reg, unsigned short value) {
	int err;
	struct i2c_rdwr_ioctl_data dat;
	unsigned addr=0x3C>>1;
	unsigned short data;
	unsigned char buf[3];
	buf[0]=reg;
	//!!!!!!
	//Endian inversion !!!!
	buf[2]=value&0xff;
	buf[1]=value>>8;
	struct i2c_msg msgs[]= {
		{
			.addr = addr,
			.flags = 0,
			.len = 3,
			.buf = buf,
		}
	};
	dat.msgs=msgs;
	dat.nmsgs=1;
	
	err=ioctl(fd, I2C_RDWR, &dat);
	if(err==-1) {
		perror("I2C_RDWR");
		return;
	}
}

unsigned short apply(unsigned short cur, char *arg) {
	if(!arg)
		return cur;
	if(arg[0]!='+' && arg[0]!='-')
		return 32-atoi(arg);
	unsigned short int offset=atoi(arg+1);
	//As 0 is maximum volume and 32 minimum, inverse + and -
	unsigned short res;
	if(arg[0]=='+')
		res=cur-offset;
	else
		res=cur+offset;
	printf("%d(%d%c%d)\n", res, cur, arg[0],offset);
	if(res>31) {
		if(arg[0]=='+')
			return 0;
		else
			return 0x1f;
	}
	return res;
}

int main(int argc, char **argv, char **envp) {
	fd=open("/dev/i2c-1", O_RDWR);
	int err;
	err=ioctl(fd, I2C_TENBIT, 0);
	if(err) {
		perror("I2C_TEN");
		return 0;
	}
	err=ioctl(fd, I2C_SLAVE, 0x3C);
	if(err) {
		perror("I2C_SLAVE");
		return 0;
	}
	if(argc<=1) {
		printf("Usage: %s [+|-|]xx\n", argv[0]);
	}

	unsigned short cur_volume;
	//For SPK
	cur_volume=read_i2c(0x02)&0xff;
	//Not muted
	if(!(cur_volume&0x80)) {
		cur_volume=apply(cur_volume, argv[1]);
		cur_volume=cur_volume<<8 | cur_volume;
		write_i2c(0x02, cur_volume);
	}

	//For HP
	cur_volume=read_i2c(0x04)&0xff;
	//Not muted
	if(!(cur_volume&0x80)) {
		cur_volume=apply(cur_volume, argv[1]);
		cur_volume=cur_volume<<8 | cur_volume;
		write_i2c(0x04, cur_volume);
	}
}
