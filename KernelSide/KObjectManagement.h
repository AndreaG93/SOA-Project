#pragma once
#include <linux/kobject.h>
#include <linux/sysfs.h>.

const struct attribute_group *allocateAttributeGroup(unsigned int attributeGroupSize, umode_t defaultAttributesMode, ...);

void createAttributeGroupSysFiles(struct kobject *parentKObject, const struct attribute_group *attributeGroup, unsigned long attributeGroupSize);

void freeAttributeGroup(const struct attribute_group *input, unsigned long attributeGroupSize);

void removeKObjectFromSystem(struct kobject *input, unsigned long attributeGroupSize);