#include "KObjectManagement.h"

struct kobject* allocateAddingToSysFilesystemKObject(struct kobject* parent, char* name) {

    return kobject_create_and_add(name, kernel_kobj);

}

const struct attribute_group *allocateAttributeGroup(unsigned int attributeGroupSize, umode_t defaultAttributesMode, ...) {

    struct attribute_group *output;
    struct attribute **attributeList;
    struct attribute* attribute;

    output = kmalloc(sizeof(const struct attribute_group), GFP_KERNEL);
    if (output == NULL)
        printk("kmalloc FAILED"); // TODO
    else {

        attributeList = kmalloc(sizeof(struct attribute *) * groupSize, GFP_KERNEL);
        if (attributeList == NULL) {

            printk("kmalloc FAILED"); // TODO
            kfree(output);
            output = NULL;

        } else {

            va_list ap;
            va_start(ap, groupSize);

            int index;
            for (index = 0; index < groupSize; index++) {

                attribute = kmalloc(sizeof(struct attribute), GFP_KERNEL);
                // TODO ERROR!!!

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

void freeKObject(struct kobject* input) {

    freeAttributeGroup(input->ktype->default_groups);
    kfree(input->ktype->sysfs_ops);
    kfree(input->ktype);
    kfree(input->name);

    kfree(input);
}

void freeAttributeGroup(const struct attribute_group *input, unsigned long attributeGroupSize) {

    unsigned long currentAttributeIndex = 0;

    for (currentAttributeIndex = 0; currentAttributeIndex < attributeGroupSize; currentAttributeIndex++)
        kfree((input->attrs + currentAttributeIndex));

    kfree(input->attrs);
    kfree(input);
}

void createAttributeGroupSysFiles(struct kobject *parentKObject, const struct attribute_group *attributeGroup, unsigned long attributeGroupSize) {

    unsigned long currentAttributeIndex = 0;

    for (currentAttributeIndex = 0; currentAttributeIndex < attributeGroupSize; currentAttributeIndex++)
        sysfs_create_file(parentKObject, *(attributeGroup->attrs + currentAttributeIndex));
}

void removeAttributeGroupSysFiles(struct kobject *parentKObject, const struct attribute_group *attributeGroup, unsigned long attributeGroupSize) {

    unsigned long currentAttributeIndex = 0;

    for (currentAttributeIndex = 0; currentAttributeIndex < attributeGroupSize; currentAttributeIndex++)
        sysfs_remove_file(parentKObject, *(attributeGroup->attrs + currentAttributeIndex));
}



