#include "internals.h"
#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
#include<linux/printk.h>


MODULE_LICENSE("GPL");


static int selfs_param = 0;
module_param(selfs_param, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(selfs_param, "selfs parameter");


extern void selfs_func(void);


static int __init selfs_init(void)
{
    pr_info("Hello, World!\n");
    pr_info("param: %d\n", selfs_param);
    selfs_func();

    return 0;
}

static void __exit selfs_exit(void)
{
    pr_info("Goodbye, World...\n");
}

module_init(selfs_init);
module_exit(selfs_exit);

