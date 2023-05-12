obj-m += studyfs.o
studyfs-objs := studyfs_main.o studyfs_super.o studyfs_hoge_dir.o studyfs_piyo_file.o

CFLAGS_studyfs_main.o := -DDEBUG
CFLAGS_studyfs_super.o := -DDEBUG
CFLAGS_studyfs_hoge_dir.o := -DDEBUG
CFLAGS_studyfs_piyo_file.o := -DDEBUG

PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

help:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) help

