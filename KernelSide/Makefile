obj-m += TimedMessagingSystemModule.o
TimedMessagingSystemModule-objs := TMSModule.o TMSDeviceDriver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean