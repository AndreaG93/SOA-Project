sudo rmmod TimedMessagingSystemModule
make clean
make
sudo insmod ./TimedMessagingSystemModule.ko
modinfo TimedMessagingSystemModule.ko
make clean

mknod /dev/TMS0 c 240 0
mknod /dev/TMS1 c 240 1
mknod /dev/TMS2 c 240 2

sudo chmod a+rw /dev/TMS0
sudo chmod a+rw /dev/TMS1
sudo chmod a+rw /dev/TMS2