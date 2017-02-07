#include <libusb.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include <string.h>

libusb_device_handle *dev;

void eprint(const char* msg, int err) {
	switch(err) {
	case LIBUSB_ERROR_IO:
		fprintf(stderr, "%s: I/O\n", msg);
		break;
	case LIBUSB_ERROR_NOT_FOUND:
		fprintf(stderr, "%s: not found\n", msg);
		break;
	case LIBUSB_ERROR_BUSY:
		fprintf(stderr, "%s: busy\n", msg);
		break;
	case LIBUSB_ERROR_NO_DEVICE:
		fprintf(stderr, "%s: no device\n", msg);
		break;
	case LIBUSB_ERROR_NOT_SUPPORTED:
		fprintf(stderr, "%s: not supported\n", msg);
		break;
	case LIBUSB_ERROR_PIPE:
		fprintf(stderr, "%s: pipe\n", msg);
		break;
	case LIBUSB_ERROR_TIMEOUT:
		fprintf(stderr, "%s: timeout\n", msg);
		break;
	default:
		fprintf(stderr, "%s: errno=%d\n", msg, err);
	}
}

int init_device() {
	int ret, cfg;

	ret = libusb_init(NULL);
	if(ret != 0) {
		fprintf(stderr, "failed to initialize libusb\n");
		return 1;
	}

	// DEBUG
	libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_ERROR);

  // TODO: Should give appropriate ID for your Alienware Corporation device via lsusb
	//dev = libusb_open_device_with_vid_pid(NULL, 0x187c, 0x0530);
	dev = libusb_open_device_with_vid_pid(NULL, 0x187c, 0x0530);
	if(dev == NULL) {
		fprintf(stderr, "failed to open usb device\n");
		goto bye_library;
	}

	ret = libusb_get_configuration(dev, &cfg);
	if(cfg != 1) {
		ret = libusb_detach_kernel_driver(dev, 0);
		if(ret != 0) {
			if(ret != LIBUSB_ERROR_NOT_FOUND) {
				eprint("failed to detach driver", ret);
				goto bye_device;
			}
		}

		ret = libusb_set_configuration(dev, 1);
		if(ret != 0) {
			eprint("failed to configure the device", ret);
			goto bye_device;
		}
	}

	ret = libusb_claim_interface(dev, 0);
	if(ret != 0) {
		eprint("failed to claim the device", ret);
		goto bye_device;
	}

	return 0;

	bye_device:
		libusb_close(dev);
	bye_library:
		libusb_exit(NULL);
		return 1;
}

int send_message(unsigned char *bytes){
	int ret;
	ret = libusb_control_transfer(dev, 0x21, 9, 0x0202, 0, bytes, 9, 0);
	if(ret < 0) {
		eprint("(reset) control_transfer failed", ret);
		return 1;
	}
	return 0;
}

void deinit_device(){
	libusb_release_interface(dev, 0);
	libusb_close(dev);
	libusb_exit(NULL);
}

unsigned char c16tob(char c){
	if('0' <= c && c <= '9')
		return c - '0';
	if('a' <= c && c <= 'f')
		return c - 'a' + 10;
	if('A' <= c && c <= 'F')
		return c - 'A' + 10;
	return 255;
}

int parse_bytes(const char *str, unsigned char *buf){
	unsigned char c1, c2;
	int i, j;

	for(i = j = 0; str[i]; j ++) {
		c1 = c16tob(str[i++]);
		c2 = c16tob(str[i++]);
		if(c1 == 255 || c2 == 255)
			return 1;

		buf[j] = 16 * c1 + c2;

		if(str[i] == ':')
			i ++;
	}

	/* pad with zero */
	for(; j < 9; j ++)
		buf[j] = 0;

	return 0;
}

int exists(const char *fname) {
    FILE *file;
    if (file = fopen(fname, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

char *trim(char *str) {
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while( isspace(*frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace(*(--endp)) && endp != frontp ) {}
    }

    if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    else if( frontp != str &&  endp == frontp )
            *str = '\0';

    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of endp to mean the front of the string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }


    return str;
}
