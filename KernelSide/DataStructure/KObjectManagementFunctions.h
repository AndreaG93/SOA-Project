#pragma once

typeder struct {

    ssize_t (*show)(struct kobject *, struct kobj_attribute *, char *);
    ssize_t (*store)(struct kobject *, struct kobj_attribute *, const char *, size_t)

} KObjectManagementFunctions;