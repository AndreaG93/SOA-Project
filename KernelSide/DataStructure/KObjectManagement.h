#pragma once

#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

struct kobject *allocateKObject(char *name, struct kobject *kObjectParent,
                                ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *),
                                ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *, size_t));

void freeKObject(struct kobject *kObject);
