#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <proto/ethernet.h>
#include <dhd_dbg.h>

#ifdef CUSTOM_MAC
static char custom_mac_file[] = "/data/local/mac.dat";

struct file* file_open(const char* path, int flags, int rights) {
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if(IS_ERR(filp)) {
    	err = PTR_ERR(filp);
    	return NULL;
    }
    return filp;
}

void file_close(struct file* file) {
    filp_close(file, NULL);
}

int file_read(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) {
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

int custom_get_mac_addr(unsigned char *buf)
{
	struct file* fp = NULL;
	int result = -EINVAL;

	fp = file_open(custom_mac_file,O_RDONLY,0);
	if (!fp)
	{
	  DHD_ERROR(("%s: can't open file %s\n",__FUNCTION__,custom_mac_file));
	  return result;
	}

	if (file_read(fp,0,buf,ETHER_ADDR_LEN) == ETHER_ADDR_LEN)
	{
	  result = 0;
	}
	else
	{
	  DHD_ERROR(("%s: can't read file %s\n",__FUNCTION__,custom_mac_file));
	}

	file_close(fp);

	return result;
}
#endif
