//===-- swordrt_rtl.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is a part of Archer/SwordRT, an OpenMP race detector.
//===----------------------------------------------------------------------===//

#ifndef SWORDRT_RTL_H
#define SWORDRT_RTL_H

#include <omp.h>
#include <ompt.h>
#include <stdio.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>

#define SWORDRT_DEBUG 	1

std::mutex pmtx;
#ifdef SWORDRT_DEBUG
#define ASSERT(x) assert(x);
#define DATA(stream, x) 										\
		do {													\
			std::unique_lock<std::mutex> lock(pmtx);			\
			stream << x << std::endl;							\
		} while(0)
#define DEBUG(stream, x) 										\
		do {													\
			std::unique_lock<std::mutex> lock(pmtx);			\
			stream << "DEBUG INFO[" << x << "][" << __FUNCTION__ << ":" << __FILE__ << ":" << std::dec << __LINE__ << "]" << std::endl;	\
		} while(0)
#else
#define ASSERT(x)
#define DEBUG(stream, x)
#endif

#define ALWAYS_INLINE			__attribute__((always_inline))
#define CALLERPC 				((size_t) __builtin_return_address(0))

#define TLS
//#define NOTLS

enum AccessSize {
	size1 = 0,
	size2,
	size4,
	size8,
	size16
};

enum AccessType {
	none = 0,
	unsafe_read,
	unsafe_write,
	atomic_read,
	atomic_write,
	mutex_read,
	mutex_write,
	nutex_read,
	nutex_write
};

struct AccessInfo
{
	size_t address;
	size_t count;
	AccessSize size;
	AccessType type;
	size_t pc;

	AccessInfo() {
		address = 0;
		count = 0;
		size = size4;
		type = none;
		pc = 0;
	}

	AccessInfo(size_t a, size_t c,	AccessSize s,
			AccessType t, size_t p) {
		address = a;
		count = c;
		size = s;
		type = t;
		pc = p;
	}
};

#if !defined(TLS) && !defined(NOTLS)
typedef struct ThreadInfo {
	size_t *stack;
	size_t stacksize;
	int __swordomp_status__;
	uint8_t __swordomp_is_critical__;
} ThreadInfo;
#endif

static ompt_get_thread_id_t ompt_get_thread_id;

#ifdef TLS
thread_local uint64_t tid = 0;
thread_local size_t *stack;
thread_local size_t stacksize;
thread_local int __swordomp_status__ = 0;
thread_local uint8_t __swordomp_is_critical__ = false;
thread_local AccessInfo accessInfo[10];
thread_local unsigned num_accesses = 1;
thread_local std::unordered_map<uint64_t, AccessInfo> accesses;
size_t barrier_id;
#elif NOTLS
extern thread_local uint64_t tid;
extern thread_local size_t *stack;
extern thread_local size_t stacksize;
extern thread_local int __swordomp_status__;
extern thread_local uint8_t __swordomp_is_critical__;
thread_local std::unordered_map<uint64_t, AccessInfo> accesses;
size_t barrier_id;
#else
#define MAX_THREADS 256
thread_local uint64_t tid;
ThreadInfo threadInfo[MAX_THREADS];
#endif

#endif  // SWORDRT_RTL_H
