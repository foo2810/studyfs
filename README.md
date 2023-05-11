studyfs
=======

What is this?
-------------
This repository is for learning file system implementations.
A simple virtual file system implementation is placed here.

Caution!!
---------
**It may break your system!!**

I cannot be held responsible for anything that happens.

Try it on a "toy system" (e.g. Linux on a virtual machine).

Compile and Install
-------------------
The filesystem is implemented as a kernel module.

```
# make

# insmod selfs.ko

# mkdir mnt
# mount -t selfs none mnt
```

Example
-------

```
# ls -l mnt
total 0
drwxr-xr-x. 2 root root 0 May 11 23:35 hoge_dir
-rw-r--r--. 1 root root 0 May 11 23:35 piyo_file

# ls -l mnt
total 0
drwxr-xr-x. 2 root root 0 May 11 23:35 hoge_dir
-rw-r--r--. 1 root root 0 May 11 23:35 piyo_file
```

License
-------
Under construction...

