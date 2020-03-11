#pragma once

#define COMPARE_AND_SWAP(dataPointer, expectedValue, desiredValue) __atomic_compare_exchange_n(dataPointer, expectedValue, desiredValue, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

#define FETCH_AND_ADD(dataPointer, value) __atomic_fetch_add(dataPointer, value, __ATOMIC_SEQ_CST)