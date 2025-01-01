// SPDX-License-Identifier: GPL-2.0-only
/*
 * ARC700 mmap
 *
 * (started from arm version - for VIPT alias handling)
 *
 * Copyright (C) 2013 Synopsys, Inc. (www.synopsys.com)
 */

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/sched/mm.h>

#include <asm/cacheflush.h>

#define COLOUR_ALIGN(addr, pgoff)			\
	((((addr) + SHMLBA - 1) & ~(SHMLBA - 1)) +	\
	 (((pgoff) << PAGE_SHIFT) & (SHMLBA - 1)))

/*
 * Ensure that shared mappings are correctly aligned to
 * avoid aliasing issues with VIPT caches.
 * We need to ensure that
 * a specific page of an object is always mapped at a multiple of
 * SHMLBA bytes.
 */
unsigned long
arch_get_unmapped_area(struct file *filp, unsigned long addr,
		unsigned long len, unsigned long pgoff, unsigned long flags)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	int do_align = 0;
	int aliasing = cache_is_vipt_aliasing();
	struct vm_unmapped_area_info info;

	/*
	 * We only need to do colour alignment if D cache aliases.
	 */
	if (aliasing)
		do_align = filp || (flags & MAP_SHARED);

	/*
	 * We enforce the MAP_FIXED case.
	 */
	 // 如果指定了 MAP_FIXED 表示必须要从指定的 addr 开始映射 len 长度的区域
    // 如果这块区域已经存在映射关系，那么后续内核会把旧的映射关系覆盖掉
	if (flags & MAP_FIXED) {
		if (aliasing && flags & MAP_SHARED &&
		    (addr - (pgoff << PAGE_SHIFT)) & (SHMLBA - 1))
			return -EINVAL;
		return addr;
	}

	if (len > TASK_SIZE)
		return -ENOMEM;

	 // 没有指定 MAP_FIXED，但是我们指定了 addr
    // 希望内核从我们指定的 addr 地址开始映射，内核这里会检查指定的这块虚拟内存范围是否有效
	if (addr) {
		if (do_align)
			addr = COLOUR_ALIGN(addr, pgoff);
		else
			addr = PAGE_ALIGN(addr); // 对齐

		// 内核这里需要确认一下指定的 [addr, addr+len] 这段虚拟内存区域是否存在已有的映射关系
        // [addr, addr+len] 地址范围内已经存在映射关系，则不能按照指定的 addr 作为映射起始地址
        // 在进程地址空间中查找第一个符合 addr < vma->vm_end  条件的 VMA
        // 如果不存在这样一个 vma（!vma）, 则表示 [addr, addr+len] 这段范围的虚拟内存是可以使用的，内核将会从我们指定的 addr 开始映射
        // 如果存在这样一个 vma ，则表示  [addr, addr+len] 这段范围的虚拟内存区域目前已经存在映射关系了，不能采用 addr 作为映射起始地址
        // 这里还有一种情况是 addr 落在 prev 和 vma 之间的一块未映射区域
        // 如果这块未映射区域的长度满足 len 大小，那么这段未映射区域可以被本次使用，内核也会从我们指定的 addr 开始映射
		vma = find_vma(mm, addr);
		if (TASK_SIZE - len >= addr &&
		    (!vma || addr + len <= vm_start_gap(vma)))
			return addr;
	}

	// 如果我们明确指定 addr 但是指定的虚拟内存范围是一段无效的区域或者已经存在映射关系
    // 那么内核会自动在地址空间中寻找一段合适的虚拟内存范围出来
    // 这段虚拟内存范围的起始地址就不是我们指定的 addr 了
	info.flags = 0;
	info.length = len;
	info.low_limit = mm->mmap_base; // 这里定义从哪里开始查找 VMA, 这里我们会从文件映射与匿名映射区开始查找
	info.high_limit = TASK_SIZE; // 查找结束位置为进程地址空间的末尾 TASK_SIZE
	info.align_mask = do_align ? (PAGE_MASK & (SHMLBA - 1)) : 0;
	info.align_offset = pgoff << PAGE_SHIFT;
	return vm_unmapped_area(&info); // 寻找未映射的虚拟内存区域

}
