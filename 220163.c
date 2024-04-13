static unsigned long mremap_to(unsigned long addr, unsigned long old_len,
		unsigned long new_addr, unsigned long new_len, bool *locked,
		unsigned long flags, struct vm_userfaultfd_ctx *uf,
		struct list_head *uf_unmap_early,
		struct list_head *uf_unmap)
{
	struct mm_struct *mm = current->mm;
	struct vm_area_struct *vma;
	unsigned long ret = -EINVAL;
	unsigned long charged = 0;
	unsigned long map_flags = 0;

	if (offset_in_page(new_addr))
		goto out;

	if (new_len > TASK_SIZE || new_addr > TASK_SIZE - new_len)
		goto out;

	/* Ensure the old/new locations do not overlap */
	if (addr + old_len > new_addr && new_addr + new_len > addr)
		goto out;

	/*
	 * move_vma() need us to stay 4 maps below the threshold, otherwise
	 * it will bail out at the very beginning.
	 * That is a problem if we have already unmaped the regions here
	 * (new_addr, and old_addr), because userspace will not know the
	 * state of the vma's after it gets -ENOMEM.
	 * So, to avoid such scenario we can pre-compute if the whole
	 * operation has high chances to success map-wise.
	 * Worst-scenario case is when both vma's (new_addr and old_addr) get
	 * split in 3 before unmaping it.
	 * That means 2 more maps (1 for each) to the ones we already hold.
	 * Check whether current map count plus 2 still leads us to 4 maps below
	 * the threshold, otherwise return -ENOMEM here to be more safe.
	 */
	if ((mm->map_count + 2) >= sysctl_max_map_count - 3)
		return -ENOMEM;

	if (flags & MREMAP_FIXED) {
		ret = do_munmap(mm, new_addr, new_len, uf_unmap_early);
		if (ret)
			goto out;
	}

	if (old_len >= new_len) {
		ret = do_munmap(mm, addr+new_len, old_len - new_len, uf_unmap);
		if (ret && old_len != new_len)
			goto out;
		old_len = new_len;
	}

	vma = vma_to_resize(addr, old_len, new_len, flags, &charged);
	if (IS_ERR(vma)) {
		ret = PTR_ERR(vma);
		goto out;
	}

	/* MREMAP_DONTUNMAP expands by old_len since old_len == new_len */
	if (flags & MREMAP_DONTUNMAP &&
		!may_expand_vm(mm, vma->vm_flags, old_len >> PAGE_SHIFT)) {
		ret = -ENOMEM;
		goto out;
	}

	if (flags & MREMAP_FIXED)
		map_flags |= MAP_FIXED;

	if (vma->vm_flags & VM_MAYSHARE)
		map_flags |= MAP_SHARED;

	ret = get_unmapped_area(vma->vm_file, new_addr, new_len, vma->vm_pgoff +
				((addr - vma->vm_start) >> PAGE_SHIFT),
				map_flags);
	if (IS_ERR_VALUE(ret))
		goto out1;

	/* We got a new mapping */
	if (!(flags & MREMAP_FIXED))
		new_addr = ret;

	ret = move_vma(vma, addr, old_len, new_len, new_addr, locked, flags, uf,
		       uf_unmap);

	if (!(offset_in_page(ret)))
		goto out;

out1:
	vm_unacct_memory(charged);

out:
	return ret;
}