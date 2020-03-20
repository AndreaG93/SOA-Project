#pragma once

#define FALSE 0
#define TRUE 1

#define FAILURE 1

#define WAKE_UP_FLAG 1
#define NOT_WAKE_UP_FLAG 0

#define DEFAULT_MAX_MESSAGE_SIZE 50
#define DEFAULT_MAX_STORAGE_SIZE 30

#define SET_SEND_TIMEOUT 5
#define SET_RECV_TIMEOUT 6
#define REVOKE_DELAYED_MESSAGES 7
#define CLEAN_QUEUE 8

typedef unsigned char DriverError;

#define SUCCESS 0
#define ERR_GENERIC_FAILURE 1
#define ERR_ALLOCATION_FAILED 2
#define ERR_EMPTY_QUEUE 3
#define ERR_FULL_QUEUE 4
#define ERR_MESSAGE_TOO_BIG 5
#define ERR_NOT_FOUND 6