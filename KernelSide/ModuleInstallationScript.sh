sudo rmmod TimedMessagingSystemModule
make clean
make
sudo insmod ./TimedMessagingSystemModule.ko
modinfo TimedMessagingSystemModule.ko
make clean

mknod /dev/TMS0 c 238 0
mknod /dev/TMS1 c 238 1
mknod /dev/TMS2 c 238 2
