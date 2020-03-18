sudo rmmod TimedMessagingSystemModule
make clean
make
sudo insmod ./TimedMessagingSystemModule.ko
modinfo TimedMessagingSystemModule.ko
make clean

mknod /dev/TMS0 c 241 0
mknod /dev/TMS1 c 241 1
mknod /dev/TMS2 c 241 2

sudo chmod a+rw /dev/TMS0
sudo chmod a+rw /dev/TMS1
sudo chmod a+rw /dev/TMS2