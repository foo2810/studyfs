obj-m += selfs.o
selfs-objs := selfs_main.o selfs_super.o selfs_hoge_dir.o

CFLAGS_selfs_main.o := -DDEBUG
CFLAGS_selfs_super.o := -DDEBUG
CFLAGS_selfs_hoge_dir.o := -DDEBUG

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

help:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) help

