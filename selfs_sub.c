#include "internals.h"
#include<linux/module.h>
#include<linux/printk.h>


MODULE_LICENSE("GPL");


void selfs_func(void)
{
    pr_info("selfs_func called\n");
}

