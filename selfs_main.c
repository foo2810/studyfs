#include "internals.h"
#include<linux/fs.h>
#include<linux/pseudo_fs.h>
#include<linux/fs_context.h>
#include<linux/fs_parser.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/printk.h>


MODULE_LICENSE("GPL");


/*
static int selfs_param = 0;
module_param(selfs_param, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(selfs_param, "selfs parameter");
*/

#define SELFS_MAGIC 0x20230507


extern struct super_operations selfs_super_ops;
extern int selfs_hoge_dir_init(struct super_block *, struct dentry *);
extern int selfs_piyo_file_init(struct super_block *, struct dentry *);



static const struct file_operations root_dir_file_ops = {
    .open = dcache_dir_open,
    .release = dcache_dir_close,
    .llseek = dcache_dir_lseek,
    .read = generic_read_dir,
    .iterate_shared = dcache_readdir,
    .fsync = noop_fsync,
};

static const struct inode_operations root_dir_inode_ops = {
    .lookup = simple_lookup,
};


static void selfs_free_fc(struct fs_context *fc)
{
    if (!fc->s_fs_info) {
        kfree(fc->s_fs_info);
        fc->s_fs_info = NULL;
    }
}

static int selfs_fill_super(struct super_block *sb, struct fs_context *fc)
{
    struct inode *inode;
	struct dentry *root;


    // configure super_block (filesystem-specific parts)
    sb->s_blocksize = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;
	sb->s_magic = SELFS_MAGIC;
	sb->s_op = &selfs_super_ops;
	sb->s_time_gran = 1;
    pr_debug("configured super_block\n");


    // create root inode and root dentry
	inode = new_inode(sb);
	if (!inode)
		return -ENOMEM;

	inode->i_ino = 1;           // inode number
	inode->i_mode = 
        S_IFDIR |               // file type: directory
        S_IRWXU |               // u+rwx
        S_IRGRP | S_IXGRP |     // g+rx
        S_IROTH | S_IXOTH;      // o+rx
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	//inode->i_op = &simple_dir_inode_operations;
	//inode->i_fop = &simple_dir_operations;
    inode->i_op = &root_dir_inode_ops;
    inode->i_fop = &root_dir_file_ops;
	set_nlink(inode, 2);    // set number of hard links (that of empty dir is 2. self and ".")
	root = d_make_root(inode);
	if (!root)
		return -ENOMEM;
    pr_debug("created root directory\n");

    // children of root
    selfs_hoge_dir_init(sb, root);
    selfs_piyo_file_init(sb, root);
    pr_debug("created children\n");

    // set root dentry ?
    sb->s_root = root;
    pr_debug("selfs_fill_super done\n");

    return 0;
}

static int selfs_get_tree(struct fs_context *fc)
{
    // get_tree_nodev: fs/super.c
    return get_tree_nodev(fc, selfs_fill_super);
}

/*** Parse Mount Options ***/
// mount options??
enum selfs_param {
    Opt_data,
};

const struct fs_parameter_spec selfs_fs_params[] = {
    fsparam_string("data", Opt_data),
    {}
};

static int selfs_parse_param(struct fs_context *fc, struct fs_parameter *param)
{
    //struct fs_parse_result result;
    //struct selfs_fs_info *fsi = fc->s_fs_info;

    // c.f. https://www.kernel.org/doc/html/latest/filesystems/mount_api.html#parameter-description
    pr_debug("data param: %s\n", param->string);

    return 0;
}
/*** End of Parse Mount Options ***/


static const struct fs_context_operations selfs_context_ops = {
    // clean up the filesystem-specific part of the fc_context
    // i.e. clean up fs_context.s_fs_info
    // c.f. Documentation/filesystems/mount_api.txt
    .free = selfs_free_fc,

    // Called when fs_context has been duplicated to duplicate the filesystem-private data.
    //.dup = ...,

    // Maybe..., called when parse mount options
    .parse_param = selfs_parse_param,

    //.parse_monolithic = ...,

    // Called to get or create the mountable root and superblock.
    // c.f. Documentation/filesystems/mount_api.txt
    .get_tree = selfs_get_tree,

    // Called when remount ??
    //.reconfigure = ...,
};


// selfs management object (unnecessary)
struct selfs_fs_info {
    int val;
};

int selfs_init_fs_context(struct fs_context *fc) {
    struct selfs_fs_info *fs_info;
    
    fs_info = kzalloc(sizeof(struct selfs_fs_info), GFP_KERNEL);
    if (!fs_info)
        return -ENOMEM;
    fs_info->val = 45;

    fc->s_fs_info = fs_info;
    fc->ops = &selfs_context_ops;
    
    return 0;
}

static void selfs_kill_sb(struct super_block *sb)
{
    if (!sb->s_fs_info) {
        kfree(sb->s_fs_info);
    }

    kill_litter_super(sb);
}

//static int selfs_fill_super (struct super_block *sb, void *data, int silent)
//{
//    // configure super_block
//    // create root directory (root inode and dentry, and children inodes and dentries)
//    
//    return 0;
//}
//
//static struct dentry *selfs_mount(struct file_system_type *fs_type,
//    int flags, const char *dev_name, void *data)
//{
//    return mount_nodev(fs_type, flags, data, selfs_fill_super);
//}

struct file_system_type selfs_fs_type = {
    .name = "selfs",
    .owner = THIS_MODULE,

    //.mount = selfs_mount,

    // Initialize filesystem-specific parts of fs_context (maybe...)
    .init_fs_context = selfs_init_fs_context,

    .parameters = selfs_fs_params,

    // Clean up super_block
    .kill_sb = selfs_kill_sb, 
    .fs_flags = FS_USERNS_MOUNT, 
    //.next = NULL,
};


static int __init selfs_init(void)
{
    pr_info("selfs module loaded\n");
    //pr_info("param: %d\n", selfs_param);

    return register_filesystem(&selfs_fs_type);
}

static void __exit selfs_exit(void)
{
    pr_info("selfs module unloaded\n");
    unregister_filesystem(&selfs_fs_type);
}

module_init(selfs_init);
module_exit(selfs_exit);

