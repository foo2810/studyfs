#include "internals.h"
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/fs_context.h>
#include<linux/printk.h>


MODULE_LICENSE("GPL");


const struct super_operations selfs_super_ops = {
    //.alloc_inode
    //.destroy_inode

    //.dirty_inode
    //.write_inode
    .drop_inode = generic_delete_inode,
    //.evict_inode
    //.put_super
    //.sync_fs
    //.freeze_super
    //.freeze_fs
    //.thaw_super
    //.unfreeze_fs
    .statfs = simple_statfs,
    //.remount_fs
    //.umount_begin

    //.show_options
    //.show_devname
    //.show_path
    //.show_stats

	//.bdev_try_to_free_page
    //.nr_cached_objects
    //.free_cached_objects
};

