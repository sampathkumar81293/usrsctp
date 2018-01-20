/*-
 * Copyright (c) 2009-2010 Brad Penoff
 * Copyright (c) 2009-2010 Humaira Kamal
 * Copyright (c) 2011-2012 Irene Ruengeler
 * Copyright (c) 2011-2012 Michael Tuexen
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _USER_ATOMIC_H_
#define _USER_ATOMIC_H_

/* __Userspace__ version of sys/i386/include/atomic.h goes here */

/* TODO In the future, might want to not use i386 specific assembly.
 *    The options include:
 *       - implement them generically (but maybe not truly atomic?) in userspace
 *       - have ifdef's for __Userspace_arch_ perhaps (OS isn't enough...)
 */

#include <stdio.h>
#include <sys/types.h>

#if defined(__Userspace_os_Darwin) || defined (__Userspace_os_Windows)
#if defined (__Userspace_os_Windows)
#define atomic_add_int(addr, val) InterlockedExchangeAdd((LPLONG)addr, (LONG)val)
#define atomic_fetchadd_int(addr, val) InterlockedExchangeAdd((LPLONG)addr, (LONG)val)
#define atomic_subtract_int(addr, val)   InterlockedExchangeAdd((LPLONG)addr,-((LONG)val))
#define atomic_cmpset_int(dst, exp, src) InterlockedCompareExchange((LPLONG)dst, src, exp)
#define SCTP_DECREMENT_AND_CHECK_REFCOUNT(addr) (InterlockedExchangeAdd((LPLONG)addr, (-1L)) == 1)
#error came to next if  block
#else
#include <libkern/OSAtomic.h>
#define atomic_add_int(addr, val) OSAtomicAdd32Barrier(val, (int32_t *)addr)
#define atomic_fetchadd_int(addr, val) OSAtomicAdd32Barrier(val, (int32_t *)addr)
#define atomic_subtract_int(addr, val) OSAtomicAdd32Barrier(-val, (int32_t *)addr)
#define atomic_cmpset_int(dst, exp, src) OSAtomicCompareAndSwapIntBarrier(exp, src, (int *)dst)
#define SCTP_DECREMENT_AND_CHECK_REFCOUNT(addr) (atomic_fetchadd_int(addr, -1) == 0)

#endif

#if defined(INVARIANTS)
#define SCTP_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t newval; \
	newval = atomic_fetchadd_int(addr, -val); \
	if (newval < 0) { \
		panic("Counter goes negative"); \
	} \
}
#else

#define SCTP_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t newval; \
	newval = atomic_fetchadd_int(addr, -val); \
	if (newval < 0) { \
		*addr = 0; \
	} \
}

#endif
#if defined(__Userspace_os_Windows)
static void atomic_init() {} /* empty when we are not using atomic_mtx */
#else
static inline void atomic_init() {} /* empty when we are not using atomic_mtx */
#endif

#else
/* Using gcc built-in functions for atomic memory operations
   Reference: http://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html
   Requires gcc version 4.1.0
   compile with -march=i486
 */
 //comment
#if 0
/*Atomically add V to *P.*/
//#define atomic_add_int(P, V)	 (void) __sync_fetch_and_add(P, V)
#define atomic_add_int(P,V)		__atomic_fetch_add(P, V, __ATOMIC_SEQ_CST)

/*Atomically subtrace V from *P.*/
//#define atomic_subtract_int(P, V) (void) __sync_fetch_and_sub(P, V)
#define atomic_subtract_int(P,V)		__atomic_fetch_sub(P, V, __ATOMIC_SEQ_CST)

/*
 * Atomically add the value of v to the integer pointed to by p and return
 * the previous value of *p.
 */
// #define atomic_fetchadd_int(p, v) __sync_fetch_and_add(p, v)
#define atomic_fetchadd_int(p, v)	__atomic_fetch_add(p, v, __ATOMIC_SEQ_CST)

/* Following explanation from src/sys/i386/include/atomic.h,
 * for atomic compare and set
 *
 * if (*dst == exp) *dst = src (all 32 bit words)
 *
 * Returns 0 on failure, non-zero on success
 */

// #define atomic_cmpset_int(dst, exp, src) __sync_bool_compare_and_swap(dst, exp, src)		 __atomic_compare_exchange_n(ptr, &_old, new, false,__ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
// #define atomic_cmpset_int(dst, exp, src)		 __atomic_compare_exchange_n(dst, &exp, src, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)

// static inline bool atomic_compare_and_set_bool(bool *variable, bool old_value, bool new_value){
//   return __atomic_compare_exchange_n (variable, &old_value, new_value, true, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
// }
                            
static inline int atomic_compare_and_set_int(int *variable, int old_value, int new_value){
  return __atomic_compare_exchange_n (variable, &old_value, new_value, 0 , __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

static inline int atomic_compare_and_set_uint32(uint32_t *variable, uint32_t old_value, uint32_t new_value){
  return __atomic_compare_exchange_n (variable, &old_value, new_value, 0 , __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

#define atomic_cmpset_int(name, old_value, new_value) atomic_compare_and_set_uint32(name, old_value, new_value)

#define SCTP_DECREMENT_AND_CHECK_REFCOUNT(addr) (atomic_fetchadd_int(addr, -1) == 1)
#if defined(INVARIANTS)
#define SCTP_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t oldval; \
	oldval = atomic_fetchadd_int(addr, -val); \
	if (oldval < val) { \
		panic("Counter goes negative"); \
	} \
}
#else
#define SCTP_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t oldval; \
	oldval = atomic_fetchadd_int(addr, -val); \
	if (oldval < val) { \
		*addr = 0; \
	} \
}


#endif
static inline void atomic_init() {} /* empty when we are not using atomic_mtx */
#endif
//comment
#endif


#if 0 /* using libatomic_ops */
#include "user_include/atomic_ops.h"

/*Atomically add incr to *P, and return the original value of *P.*/
#define atomic_add_int(P, V)	 AO_fetch_and_add((AO_t*)P, V)

#define atomic_subtract_int(P, V) AO_fetch_and_add((AO_t*)P, -(V))

/*
 * Atomically add the value of v to the integer pointed to by p and return
 * the previous value of *p.
 */
#define atomic_fetchadd_int(p, v) AO_fetch_and_add((AO_t*)p, v)

/* Atomically compare *addr to old_val, and replace *addr by new_val
   if the first comparison succeeds.  Returns nonzero if the comparison
   succeeded and *addr was updated.
*/
/* Following Explanation from src/sys/i386/include/atomic.h, which
   matches that of AO_compare_and_swap above.
 * Atomic compare and set, used by the mutex functions
 *
 * if (*dst == exp) *dst = src (all 32 bit words)
 *
 * Returns 0 on failure, non-zero on success
 */

#define atomic_cmpset_int(dst, exp, src) AO_compare_and_swap((AO_t*)dst, exp, src)

static inline void atomic_init() {} /* empty when we are not using atomic_mtx */
#endif /* closing #if for libatomic */

#if 1 /* using atomic_mtx */

#include <pthread.h>

extern userland_mutex_t atomic_mtx;

#if defined (__Userspace_os_Windows)
static inline void atomic_init() {
	InitializeCriticalSection(&atomic_mtx);
}
static inline void atomic_destroy() {
	DeleteCriticalSection(&atomic_mtx);
}
static inline void atomic_lock() {
	EnterCriticalSection(&atomic_mtx);
}
static inline void atomic_unlock() {
	LeaveCriticalSection(&atomic_mtx);
}
#else
static inline void atomic_init() {
	pthread_mutexattr_t mutex_attr;

	pthread_mutexattr_init(&mutex_attr);
#ifdef INVARIANTS
	pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
#endif
	pthread_mutex_init(&accept_mtx, &mutex_attr);
	pthread_mutexattr_destroy(&mutex_attr);
}
static inline void atomic_destroy() {
	(void)pthread_mutex_destroy(&atomic_mtx);
}
static inline void atomic_lock() {
#ifdef INVARIANTS
	KASSERT(pthread_mutex_lock(&atomic_mtx) == 0, ("atomic_lock: atomic_mtx already locked"))
#else
	(void)pthread_mutex_lock(&atomic_mtx);
#endif
}
static inline void atomic_unlock() {
#ifdef INVARIANTS
	KASSERT(pthread_mutex_unlock(&atomic_mtx) == 0, ("atomic_unlock: atomic_mtx not locked"))
#else
	(void)pthread_mutex_unlock(&atomic_mtx);
#endif
}
#endif
/*
 * For userland, always use lock prefixes so that the binaries will run
 * on both SMP and !SMP systems.
 */

#define SCTP_DECREMENT_AND_CHECK_REFCOUNT(addr) (atomic_fetchadd_int((u_int *)(addr), -1) == 0)
#define SCTP_SAVE_ATOMIC_DECREMENT(addr, val) \
{ \
	int32_t newval; \
	newval = atomic_fetchadd_int(addr, -val); \
	if (newval < 0) { \
		*addr = 0; \
	} \
}

#define	MPLOCKED	"lock ; "



// static __inline uint64_t
// atomic_load_acq_64_i386(volatile uint64_t *p)
// {
// 	volatile uint32_t *q;
// 	uint64_t res;

// 	q = (volatile uint32_t *)p;
// 	__asm __volatile(
// 	"	pushfl ;		"
// 	"	cli ;			"
// 	"	movl	%1,%%eax ;	"
// 	"	movl	%2,%%edx ;	"
// 	"	popfl"
// 	: "=&A" (res)			/* 0 */
// 	: "m" (*q),			/* 1 */
// 	  "m" (*(q + 1))		/* 2 */
// 	: "memory");
// 	return (res);
// }



// static __inline int
// atomic_cmpset_int(volatile u_int *dst, u_int expect, u_int src)
// {
// 	volatile u_int *p;
// 	u_char res;

// 	p = (volatile uint32_t *)dst;
//  __asm__ __volatile__ (
// 	"	pushfl ;		"
// 	"	cli ;			"
// 	"	xorl	%1,%%eax ;	"
// 	"	xorl	%2,%%edx ;	"
// 	"	orl	%%edx,%%eax ;	"
// 	"	jne	1f ;		"
// 	"	movl	%4,%1 ;		"
// 	"	movl	%5,%2 ;		"
// 	"1:				"
// 	"	sete	%3 ;		"
// 	"	popfl"
// 	: "+A" (expect),		/* 0 */
// 	  "+m" (*p),			/* 1 */
// 	  "+m" (*(p + 1)),		/* 2 */
// 	  "=q" (res)			/* 3 */
// 	: "r" (src)				/* 4 */
// 	: "memory", "cc");
// 		atomic_unlock();
// 	return (res);
// }


static __inline int
atomic_cmpset_int(volatile u_int *dst, u_int expect, u_int src)
{
 /* Atomic compare and set, used by the mutex functions
 *
 * if (*dst == exp) *dst = src (all 32 bit words)
 *
 * Returns 0 on failure, non-zero on success
 */

int ret=0;
 atomic_lock();

 if (*(u_int *)(dst) == expect) {
	*(u_int *)(dst) = src;
	ret =1;
}
 else ret=1;
 
 atomic_unlock();

 return ret;
}





static __inline int
atomic_fetchadd_int(volatile u_int *p, u_int v)
{

	for (;;) {
		// u_int t = *p;
		u_int t = *(u_int *)(p);
		if (atomic_cmpset_int(p, t, t + v))
			return ((u_int)t);
	}
}





#define atomic_add_int(P, V)	 do {   \
		atomic_lock();          \
		(*(u_int *)(P) += (V)); \
		atomic_unlock();        \
} while(0)
#define atomic_subtract_int(P, V)  do {   \
		atomic_lock();            \
		(*(u_int *)(P) -= (V));   \
		atomic_unlock();          \
} while(0)

#endif
#endif
