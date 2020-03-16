#pragma once
#include <linux/kobject.h>
#include <linux/sysfs.h>

const struct attribute_group *allocateAttributeGroup(umode_t defaultAttributesMode, unsigned int attributeGroupSize, ...);

void createAttributeGroupSysFiles(struct kobject *parentKObject, unsigned long attributeGroupSize);

void freeAttributeGroup(const struct attribute_group *input, unsigned long attributeGroupSize);

void freeKObjectRemovingFromSystem(struct kobject *input, unsigned long attributeGroupSize);
