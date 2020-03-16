#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include "KObjectManagement.h"
#include "ModuleMetadata.h"

void removeAttributeGroupSysFiles(struct kobject *parentKObject, const struct attribute_group *attributeGroup, unsigned long attributeGroupSize) {

    unsigned long currentAttributeIndex = 0;

    for (currentAttributeIndex = 0; currentAttributeIndex < attributeGroupSize; currentAttributeIndex++)
        sysfs_remove_file(parentKObject, *(attributeGroup->attrs + currentAttributeIndex));
}

const struct attribute_group *allocateAttributeGroup(umode_t defaultAttributesMode, unsigned int attributeGroupSize, ...) {

    struct attribute_group *output;
    struct attribute **attributeList;
    struct attribute *attribute;
    int index;

    output = kmalloc(sizeof(const struct attribute_group), GFP_KERNEL);
    if (output == NULL)
        printk("'%s': 'const struct attribute_group' allocation failed!\n", MODULE_NAME);
    else {

        attributeList = kmalloc(sizeof(struct attribute *) * attributeGroupSize, GFP_KERNEL);
        if (attributeList == NULL) {

            printk("'%s': 'struct attribute *' allocation failed!\n", MODULE_NAME);
            kfree(output);

            return NULL;

        } else {

            va_list ap;
            va_start(ap, attributeGroupSize);

            for (index = 0; index < attributeGroupSize; index++) {

                attribute = kmalloc(sizeof(struct attribute), GFP_KERNEL);
                if (attribute == NULL) {

                    printk("'%s': 'struct attribute' allocation failed!\n", MODULE_NAME);
                    for (index--; index >= 0; index--)
                        kfree(*(attributeList + index));

                    kfree(output);
                    return NULL;
                }

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

void freeAttributeGroup(const struct attribute_group *input, unsigned long attributeGroupSize) {

    unsigned long currentAttributeIndex = 0;

    for (currentAttributeIndex = 0; currentAttributeIndex < attributeGroupSize; currentAttributeIndex++)
        kfree((input->attrs + currentAttributeIndex));

    kfree(input->attrs);
    kfree(input);
}

void createAttributeGroupSysFiles(struct kobject *parentKObject, unsigned long attributeGroupSize) {

    unsigned long currentAttributeIndex;
    const struct attribute_group *attributeGroup;

    attributeGroup = parentKObject->ktype->default_groups[0];

    for (currentAttributeIndex = 0; currentAttributeIndex < attributeGroupSize; currentAttributeIndex++)
        sysfs_create_file(parentKObject, *(attributeGroup->attrs + currentAttributeIndex));
}

void freeKObjectRemovingFromSystem(struct kobject *input, unsigned long attributeGroupSize) {

    removeAttributeGroupSysFiles(input, input->ktype->default_groups[0], attributeGroupSize);
    kobject_put(input);

    freeAttributeGroup(input->ktype->default_groups[0], attributeGroupSize);

    kfree(input->ktype->default_groups);
    kfree(input->ktype->sysfs_ops);
    kfree(input->ktype);
    kfree(input->name);

    kfree(input);
}