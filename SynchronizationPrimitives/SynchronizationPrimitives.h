#pragma once

#define CAS(ptr, cmp, val) __atomic_compare_exchange_n(ptr, cmp, val, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED)

#define FAA(ptr, val) __atomic_fetch_add(ptr, val, __ATOMIC_RELAXED)