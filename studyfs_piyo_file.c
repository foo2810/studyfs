#include "internals.h"
#include<linux/fs.h>
#include<linux/module.h>
#include<linux/printk.h>
#include<linux/string.h>

#include<linux/uaccess.h>


MODULE_LICENSE("GPL");


static const struct file_operations piyo_file_file_ops;

static int piyo_file_permission(struct inode *inode, int mask)
{
    return generic_permission(inode, mask);
    //return inode_permission;
}

static int piyo_file_setattr(struct dentry *dentry, struct iattr *attr)
{
    return -EPERM;
}

static int piyo_file_getattr(const struct path *path, struct kstat *stat, 
    u32 request_mask, unsigned int query_flags)
{
    struct inode *inode = d_inode(path->dentry);
    generic_fillattr(inode, stat);
    return 0;

    /*** Memo ***/
    //return vfs_getattr_nosec(path, stat, request_mask, query_flags);
    //return vfs_getattr(path, stat, request_mask, query_flags);
    /*** End ***/
}

static ssize_t piyo_file_listxattr(struct dentry *dentry, char *list, size_t size)
{
    return -EOPNOTSUPP;
}

static const struct inode_operations piyo_file_inode_ops = {
    //.default_file_ops = &piyo_file_file_ops;  // not necessary??
    .link = NULL,          // hard link not supported
    .unlink = NULL,        // ???
    .symlink = NULL,       // sym link not supported
    .permission = piyo_file_permission, 
    .setattr = piyo_file_setattr, 
    .getattr = piyo_file_getattr,
    .listxattr = piyo_file_listxattr,
};

static loff_t piyo_file_llseek(struct file *file, loff_t offset, int whence)
{
    switch (whence) {
        case 1:    // SEEK_CUR
            offset += file->f_pos;
        case 0:    // SEEK_SET
            if (offset >= 0)
                break;
        default:
            return -EINVAL;
    }

    file->f_pos = offset;

    return offset;
}

static ssize_t piyo_file_read(struct file *file, char __user *buf, 
    size_t size, loff_t *ppos)
{
    const char *data = "Hello, World!\n";
    const size_t len_data = 14;
    size_t read_size;

    // Need error check?
    //strncpy(buf, data, 32);

    pr_debug("read: size=%ld, ppos=%lld\n", size, *ppos);

    if (*ppos >= len_data) {
        return 0;
    } else {
        if (*ppos + size >= len_data)
            read_size = len_data - *ppos;
        else 
            read_size = size;
    }

    pr_debug("read: read_size=%ld\n", read_size);

    if (copy_to_user(buf, data + *ppos, read_size))
        return -EFAULT;
    *ppos += read_size;
    return read_size;
}

//static ssize_t piyo_file_write(struct file *file, char __user *buf, 
//    size_t size, loff_t *ppos)
//{
//}

static int piyo_file_open(struct inode *inode, struct file *file)
{
    // (Optional) configured file->private_data;
    return 0;
}

static int piyo_file_release(struct inode *inode, struct file *file)
{
    // (Optional) cleanup file->private_data;
    return 0;
}

static const struct file_operations piyo_file_file_ops = {
    .llseek = piyo_file_llseek, 
    .read = piyo_file_read,
    //.write = piyo_file_write,
    .open = piyo_file_open,
    .release = piyo_file_release,
    .fsync = noop_fsync,
};

static void make_piyo_file_inode(struct inode *inode)
{
    set_nlink(inode, 1);
    inode->i_mode = 
        S_IFREG |       // file type: normal file
        S_IRUGO |       // ugo+r
        S_IWUSR;        // u+w
    inode->i_uid = GLOBAL_ROOT_UID;
    inode->i_gid = GLOBAL_ROOT_GID;
    inode->i_rdev = 0;
    inode->i_size = 0;
    inode->i_blkbits = PAGE_SHIFT;
    inode->i_blocks = 0;

    inode->i_op = &piyo_file_inode_ops;
    inode->i_opflags &= ~(IOP_XATTR | IOP_LOOKUP);
    inode->i_fop = &piyo_file_file_ops;
}

int studyfs_piyo_file_init(struct super_block *sb, struct dentry *root)
{
    struct inode *child_inode;
	struct dentry *child_dentry;

    child_dentry = d_alloc_name(root, "piyo_file");
    if (!child_dentry)
        return -ENOMEM;

    child_inode = new_inode(sb);
    if (!child_inode)
        return -ENOMEM;

    make_piyo_file_inode(child_inode);
    d_add(child_dentry, child_inode);

    return 0;
}

