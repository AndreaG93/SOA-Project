#pragma once

#include <linux/module.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andrea Graziani <andrea.graziani93@outlook.it>");
MODULE_VERSION("1.0a");
MODULE_DESCRIPTION("A simple system service that allows exchanging messages across threads.");

int init_module(void);

void cleanup_module(void);