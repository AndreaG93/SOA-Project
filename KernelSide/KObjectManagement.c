#include "KObjectManagement.h"
#include "./KernelLogManagement.h"
#include <linux/kobject.h>
#include <linux/sysfs.h>.

void freeKObject(struct kobject *input) {

    freeAttributeGroup(input->ktype->default_groups);
    kfree(input->ktype->sysfs_ops);
    kfree(input->ktype);
    kfree(input->name);

    kfree(input);
}

void removeAttributeGroupSysFiles(struct kobject *parentKObject, const struct attribute_group *attributeGroup, unsigned long attributeGroupSize) {

    unsigned long currentAttributeIndex = 0;

    for (currentAttributeIndex = 0; currentAttributeIndex < attributeGroupSize; currentAttributeIndex++)
        sysfs_remove_file(parentKObject, *(attributeGroup->attrs + currentAttributeIndex));
}

const struct attribute_group *allocateAttributeGroup(unsigned int attributeGroupSize, umode_t defaultAttributesMode, ...) {

    struct attribute_group *output;
    struct attribute **attributeList;
    struct attribute *attribute;

    output = kmalloc(sizeof(const struct attribute_group), GFP_KERNEL);
    if (output == NULL)
        printWarningMessageToKernelLog("'struct attribute_group' allocation failed!");
    else {

        attributeList = kmalloc(sizeof(struct attribute *) * groupSize, GFP_KERNEL);
        if (attributeList == NULL) {

            printWarningMessageToKernelLog("'struct attribute *' allocation failed!");
            kfree(output);
            return NULL;

        } else {

            va_list ap;
            va_start(ap, groupSize);

            int index;
            for (index = 0; index < groupSize; index++) {

                attribute = kmalloc(sizeof(struct attribute), GFP_KERNEL);
                if (attribute == NULL) {

                    printWarningMessageToKernelLog("'struct attribute' allocation failed!");
                    for (index--; index >= 0; index--)
                        kfree(*(attributeList + index));

                    kfree(output);
                    return NULL;
                }

                attribute->name = va_arg(ap,
                char *);
                attribute->mode = defaultAttributesMode;

                *(attributeList + index) = attribute;
            }

            va_end(ap);

            output->name = "AttributesGroup";
            output->attrs = attributeList;

            return output;
        }
    }
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

void removeKObjectFromSystem(struct kobject *input, unsigned long attributeGroupSize) {

    removeAttributeGroupSysFiles(input, input->ktype->default_groups[0], attributeGroupSize);
    kobject_put(input);

    freeAttributeGroup(input->ktype->default_groups[0]);
    freeKObject(input);
}