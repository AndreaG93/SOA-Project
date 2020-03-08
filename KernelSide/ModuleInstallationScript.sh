make clean
make
sudo insmod ./TimedMessagingSystemModule.ko
modinfo TimedMessagingSystemModule.ko
make clean