
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "KObjectManagement.h"

struct kobject *allocateKObject(char *name, struct kobject *kObjectParent,
                                ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *),
                                ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *, size_t)) {

    struct kobject *kObject;

    struct attribute_group *attributeGroup;
    struct attribute **attributes;

    struct kobj_attribute *kObjectAttribute1;
    struct kobj_attribute *kObjectAttribute2;

    kObjectAttribute1 = kmalloc(sizeof(struct kobj_attribute), GFP_KERNEL);
    if (kObjectAttribute1 == NULL)
        return NULL;

    kObjectAttribute2 = kmalloc(sizeof(struct kobj_attribute), GFP_KERNEL);
    if (kObjectAttribute2 == NULL) {

        kfree(kObjectAttribute1);

        return NULL;
    }

    kObjectAttribute1->attr.name = "max_message_size";
    kObjectAttribute1->attr.mode = 0644;
    kObjectAttribute1->show = show;
    kObjectAttribute1->store = store;

    kObjectAttribute2->attr.name = "max_storage_size";
    kObjectAttribute2->attr.mode = 0644;
    kObjectAttribute2->show = show;
    kObjectAttribute2->store = store;

    attributes = kmalloc(sizeof(struct attribute *) * 3, GFP_KERNEL);
    if (attributes == NULL) {

        kfree(kObjectAttribute1);
        kfree(kObjectAttribute2);

        return NULL;
    }

    attributes[0] = &(kObjectAttribute1->attr);
    attributes[1] = &(kObjectAttribute2->attr);
    attributes[2] = NULL;

    attributeGroup = kmalloc(sizeof(struct attribute_group), GFP_KERNEL);
    if (attributeGroup == NULL) {

        kfree(kObjectAttribute1);
        kfree(kObjectAttribute2);
        kfree(attributes);
        return NULL;
    }

    attributeGroup->attrs = attributes;
    attributeGroup->name = "SemiLockFreeQueue";

    kObject = kobject_create_and_add(name, kObjectParent);
    if (kObject == NULL) {

        kfree(attributeGroup);
        kfree(attributes);
        kfree(kObjectAttribute1);
        kfree(kObjectAttribute2);

        return NULL;
    }

    if (sysfs_create_group(kObject, attributeGroup) != 0) {

        kobject_put(kObject);
        return NULL;
    }

    return kObject;
}

void freeKObject(struct kobject *kObject) {
    kobject_put(kObject);
}