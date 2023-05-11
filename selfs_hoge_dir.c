#include "internals.h"
#include<linux/fs.h>
#include<linux/module.h>
#include<linux/printk.h>


MODULE_LICENSE("GPL");


static struct dentry *hoge_dir_lookup(struct inode *dir, struct dentry *dentry, unsigned int flags)
{
	return ERR_PTR(-ENOENT);
}

static int hoge_dir_getattr(const struct path *path, struct kstat *stat,
			     u32 request_mask, unsigned int query_flags)
{
	struct inode *inode = d_inode(path->dentry);
	generic_fillattr(inode, stat);
	return 0;
}

static int hoge_dir_setattr(struct dentry *dentry, struct iattr *attr)
{
	return -EPERM;
}

static ssize_t hoge_dir_listxattr(struct dentry *dentry, char *list, size_t size)
{
	return -EOPNOTSUPP;
}

static const struct inode_operations empty_dir_inode_operations = {
	.lookup = hoge_dir_lookup,
    .mkdir = NULL,      // mkdir not supported
    .rmdir = NULL,      // rmdir not supported
	.permission = generic_permission,
	.setattr = hoge_dir_setattr,
	.getattr = hoge_dir_getattr,
	.listxattr = hoge_dir_listxattr,
};

static loff_t empty_dir_llseek(struct file *file, loff_t offset, int whence)
{
	/* An empty directory has two entries . and .. at offsets 0 and 1 */
	return generic_file_llseek_size(file, offset, whence, 2, 2);
}

static int empty_dir_readdir(struct file *file, struct dir_context *ctx)
{
	dir_emit_dots(file, ctx);
	return 0;
}

static const struct file_operations empty_dir_operations = {
	.llseek		= empty_dir_llseek,
	.read		= generic_read_dir,
	.iterate_shared	= empty_dir_readdir,
	.fsync		= noop_fsync,
};

static void make_hoge_dir_inode(struct inode *inode)
{
	set_nlink(inode, 2);
	inode->i_mode = 
        S_IFDIR |               // file type: directory
        S_IRWXU |               // u+rwx
        S_IRGRP | S_IXGRP |     // g+rx
        S_IROTH | S_IXOTH;      // o+rx
	inode->i_uid = GLOBAL_ROOT_UID;
	inode->i_gid = GLOBAL_ROOT_GID;
	inode->i_rdev = 0;
	inode->i_size = 0;
	inode->i_blkbits = PAGE_SHIFT;
	inode->i_blocks = 0;

	inode->i_op = &empty_dir_inode_operations;
	inode->i_opflags &= ~IOP_XATTR;
	inode->i_fop = &empty_dir_operations;
}


int selfs_hoge_dir_init(struct super_block *sb, struct dentry *root)
{
    struct inode *child_inode;
	struct dentry *child_dentry;

    child_dentry = d_alloc_name(root, "hoge_dir");
    if (!child_dentry)
        return -ENOMEM;

    child_inode = new_inode(sb);
    if (!child_inode)
        return -ENOMEM;

    make_hoge_dir_inode(child_inode);
    d_add(child_dentry, child_inode);

    return 0;
}

