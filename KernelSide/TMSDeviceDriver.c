#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#include "RCURedBlackTree.h"
#include "./Common.h"
#include "./TMSDeviceDriver.h"
#include <linux/kobject.h>
#include <linux/sysfs.h>.


static int majorNumber;
//static struct cdev *charDeviceFile;
//static dev_t devNo;

static char *kernelBuffer;

static unsigned int max_message_size = 8;

static RCURedBlackTree *RCUTree;

static struct kobject *example_kobject_parent;

static struct kobject *example_kobject;

static struct kobj_attribute *kObjectDriverAttribute;

static struct attribute *driverAttribute;

static struct kobj_type *ktype;


static int var = 9;


static ssize_t
var_show(struct kobject *kobj, struct attribute *attr, char *buf) {//just one page of storage from buff

    return sprintf(buf, "%d\n", var);
}

static ssize_t var_store(struct kobject *kobj, struct attribute *attr, const char *buf,
                         size_t count) {//just one page of storage from buff

    sscanf(buf, "%du", &var);
    return count;
}


static int TMS_open(struct inode *inode, struct file *file) {

    printk(KERN_DEBUG
    "'%s': 'TMS_open' function is been called!\n", DEVICE_DRIVER_NAME);

    //unsigned int mdsa = iminor(file->f_inode);
    //unsigned int mdsa = iminor(inode);

    printk(KERN_DEBUG
    "'%s': 'TMS_open' function is been called on minor number %d   e %d!\n", DEVICE_DRIVER_NAME, iminor(inode), iminor(
            file->f_inode));


    return 0;
}

static ssize_t TMS_read(struct file *file, char *userBuffer, size_t size, loff_t *offset) {

    printk(KERN_DEBUG
    "'%s': 'TMS_read' function is been called on device file (%d %d)!\n", DEVICE_DRIVER_NAME, imajor(
            file->f_inode), iminor(file->f_inode));

    unsigned long bytesNotCopied;

    void *data = search(RCUTree, 0);
    if (data != NULL) {
        bytesNotCopied = copy_to_user(userBuffer, data, 6);
    }

    return 0;
}

static ssize_t TMS_write(struct file *file, const char *buffer, size_t size, loff_t *offset) {
    return 1;
}


static int TMS_release(struct inode *inode, struct file *file) {

    printk(KERN_DEBUG
    "'%s': 'TMS_release' function is been called!\n", DEVICE_DRIVER_NAME);

    return 0;
}


static int TMS_flush(struct file *file, fl_owner_t id) {

    printk(KERN_DEBUG
    "'%s': 'TMS_flush' function is been called!\n", DEVICE_DRIVER_NAME);

    return 0;
}

static long TMS_unlocked_ioctl(struct file *file, unsigned int x, unsigned long y) {
    return 1;
}

static struct file_operations TimedMsgServiceOperation = {
        read: TMS_read,
        write: TMS_write,
        open: TMS_open,
        release: TMS_release,
        unlocked_ioctl: TMS_unlocked_ioctl,
        flush: TMS_flush
};


const struct attribute_group *
allocateKObjectAttributeGroup(unsigned int groupSize, umode_t defaultAttributesMode, ...) {

    struct attribute_group *output;
    struct attribute **attributeList;
    struct attribute* attribute;

    output = kmalloc(sizeof(const struct attribute_group), GFP_KERNEL);
    if (output == NULL)
        printk("kmalloc FAILED");
    else {

        attributeList = kmalloc(sizeof(struct attribute *) * groupSize, GFP_KERNEL);
        if (attributeList == NULL) {

            printk("kmalloc FAILED");
            kfree(output);
            output = NULL;

        } else {

            va_list ap;
            va_start(ap, groupSize);

            int index;
            for (index = 0; index < groupSize; index++) {

                attribute = kmalloc(sizeof(struct attribute), GFP_KERNEL);

                attribute->name = va_arg(ap, char *);
                attribute->mode = defaultAttributesMode;

                *(attributeList + index) = attribute;
            }

            va_end(ap);
            
            output->name = "AttributesGroup";
            output->attrs = attributeList;
        }
    }

    return output;
}

struct sysfs_ops *allocateSysFileSystemOperation(void) {

    struct sysfs_ops *output;

    output = kmalloc(sizeof(struct sysfs_ops), GFP_KERNEL);
    if (output == NULL) {
        printk("kmalloc FAILED");
    } else {
        output->show = var_show;
        output->store = var_store;
    }

    return output;
}


int registerTMSDeviceDriver(void) {

    majorNumber = register_chrdev(0, DEVICE_DRIVER_NAME, &TimedMsgServiceOperation);
    if (majorNumber < 0) {
        printk(KERN_WARNING
        "'%s' char device registration failed!\n", DEVICE_DRIVER_NAME);
        return FAILURE;
    } else {
        printk(KERN_NOTICE
        "'%s' char device is been successfully registered with major number %d!\n", DEVICE_DRIVER_NAME, majorNumber);




        /*
        charDeviceFile = cdev_alloc();
        cdev_init(charDeviceFile, &TimedMsgServiceOperation);


        charDeviceFile->ops = &TimedMsgServiceOperation;
        charDeviceFile->owner = THIS_MODULE;

        devNo = MKDEV(majorNumber, 0);
        int error = cdev_add(charDeviceFile, devNo, 0);
        if(error) {
            printk(KERN_WARNING "Error adding device");
        }
*/

        RCUTree = kmalloc(sizeof(RCUTree), GFP_KERNEL);
        RCUTree->rb_node = NULL;

        kernelBuffer = kmalloc(6 * sizeof(char), GFP_KERNEL);
        if (kernelBuffer == NULL) {
            printk(KERN_WARNING
            "'%s' allocation failed!\n", DEVICE_DRIVER_NAME);
        }

        strcpy(kernelBuffer, "Andrea");

        //RCUTree = &RB_ROOT;

        if (RCUTree == NULL)
            printk(KERN_WARNING
        "'%s' allocation failed!\n", DEVICE_DRIVER_NAME);

        insert(RCUTree, 0, kernelBuffer);
        insert(RCUTree, 1, kernelBuffer);

        printk(KERN_WARNING
        "'%s' '%s'!\n", DEVICE_DRIVER_NAME, search(RCUTree, 0));
        printk(KERN_WARNING
        "'%s' '%s'!\n", DEVICE_DRIVER_NAME, search(RCUTree, 1));


        ktype = kmalloc(sizeof(struct kobj_type), GFP_KERNEL);
        const struct attribute_group *group = allocateKObjectAttributeGroup(2, S_IWUSR | S_IRWXG, "attr1", "attr2");
        ktype->default_groups = &group;
        ktype->sysfs_ops = allocateSysFileSystemOperation();


        /*
        kObjectDriverAttribute = kmalloc(sizeof(struct kobj_attribute), GFP_KERNEL);
        kObjectDriverAttribute->attr = driverAttribute[0];
        kObjectDriverAttribute->show = var_show;
        kObjectDriverAttribute->store = var_store;
*/

        example_kobject_parent = kobject_create_and_add("you1", kernel_kobj);

        example_kobject = kobject_create_and_add("you2", example_kobject_parent);
        example_kobject->ktype = ktype;


        sysfs_create_file(example_kobject, example_kobject->ktype->default_groups[0]->attrs[0]);
        sysfs_create_file(example_kobject, example_kobject->ktype->default_groups[0]->attrs[1]);

        //sysfs_create_group(example_kobject, group);

        //sysfs_create_file(example_kobject, &kObjectDriverAttribute->attr);
        //sysfs_create_file(example_kobject, &kObjectDriverAttribute->attr);

        return SUCCESS;
    }
}

void unregisterTMSDeviceDriver(void) {

    //"mknod /dev/TMS c 238 0"
    // rm -rF /dev/TMS
    //cdev_del(charDeviceFile);

    unregister_chrdev(majorNumber, DEVICE_DRIVER_NAME);
    printk(KERN_NOTICE
    "'%s' char device is been successfully unregistered!\n", DEVICE_DRIVER_NAME);

    if (kernelBuffer)
        kfree(kernelBuffer);

    kobject_put(example_kobject);

}