obj-m += TimedMessagingSystemModule.o
TimedMessagingSystemModule-objs := TMSModule.o TMSDeviceDriver.o MessagesManagement.o CleaningFunctions.o DelayedOperations.o DataStructure/DeviceFileInstance.o DataStructure/KObjectManagement.o DataStructure/Message.o DataStructure/RBTree.o DataStructure/RCUSynchronizer.o DataStructure/SemiLockFreeQueue.o DataStructure/Session.o Common/BasicOperations.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean