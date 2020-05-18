#pragma once

#define COMPARE_AND_SWAP(dataPointer, expectedValue, desiredValue) __sync_bool_compare_and_swap(dataPointer, expectedValue, desiredValue)

#define ADD_AND_FETCH(dataPointer, value) __sync_add_and_fetch(dataPointer, value)

#define SUB_AND_FETCH(dataPointer, value) __sync_sub_and_fetch(dataPointer, value)