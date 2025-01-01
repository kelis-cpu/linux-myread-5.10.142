/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_MMAN_H
#define _UAPI_LINUX_MMAN_H

#include <asm/mman.h>
#include <asm-generic/hugetlb_encode.h>

#define MREMAP_MAYMOVE		1
#define MREMAP_FIXED		2
#define MREMAP_DONTUNMAP	4

#define OVERCOMMIT_GUESS		0 // 默认策略，进程对虚拟内存的申请不能超过物理内存总大小和 swap 交换区的总大小 之和
#define OVERCOMMIT_ALWAYS		1 // 无论进程申请多大的虚拟内存，只要不超过整个进程虚拟内存空间的大小，内核总会痛快的答应
#define OVERCOMMIT_NEVER		2 // 严格的规定虚拟内存的申请用量,般情况下为 (总物理内存大小 - 大页占用的内存大小） * 50% + swap 交换区总大小

#define MAP_SHARED	0x01		/* Share changes */
#define MAP_PRIVATE	0x02		/* Changes are private */
#define MAP_SHARED_VALIDATE 0x03	/* share + validate extension flags */

/*
 * Huge page size encoding when MAP_HUGETLB is specified, and a huge page
 * size other than the default is desired.  See hugetlb_encode.h.
 * All known huge page size encodings are provided here.  It is the
 * responsibility of the application to know which sizes are supported on
 * the running system.  See mmap(2) man page for details.
 */
#define MAP_HUGE_SHIFT	HUGETLB_FLAG_ENCODE_SHIFT
#define MAP_HUGE_MASK	HUGETLB_FLAG_ENCODE_MASK

#define MAP_HUGE_16KB	HUGETLB_FLAG_ENCODE_16KB
#define MAP_HUGE_64KB	HUGETLB_FLAG_ENCODE_64KB
#define MAP_HUGE_512KB	HUGETLB_FLAG_ENCODE_512KB
#define MAP_HUGE_1MB	HUGETLB_FLAG_ENCODE_1MB
#define MAP_HUGE_2MB	HUGETLB_FLAG_ENCODE_2MB
#define MAP_HUGE_8MB	HUGETLB_FLAG_ENCODE_8MB
#define MAP_HUGE_16MB	HUGETLB_FLAG_ENCODE_16MB
#define MAP_HUGE_32MB	HUGETLB_FLAG_ENCODE_32MB
#define MAP_HUGE_256MB	HUGETLB_FLAG_ENCODE_256MB
#define MAP_HUGE_512MB	HUGETLB_FLAG_ENCODE_512MB
#define MAP_HUGE_1GB	HUGETLB_FLAG_ENCODE_1GB
#define MAP_HUGE_2GB	HUGETLB_FLAG_ENCODE_2GB
#define MAP_HUGE_16GB	HUGETLB_FLAG_ENCODE_16GB

#endif /* _UAPI_LINUX_MMAN_H */
