static int __do_fault(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, pmd_t *pmd,
		pgoff_t pgoff, unsigned int flags, pte_t orig_pte)
{
	pte_t *page_table;
	spinlock_t *ptl;
	struct page *page;
	pte_t entry;
	int anon = 0;
	struct page *dirty_page = NULL;
	struct vm_fault vmf;
	int ret;
	int page_mkwrite = 0;

	vmf.virtual_address = (void __user *)(address & PAGE_MASK);
	vmf.pgoff = pgoff;
	vmf.flags = flags;
	vmf.page = NULL;

	ret = vma->vm_ops->fault(vma, &vmf);
	if (unlikely(ret & (VM_FAULT_ERROR | VM_FAULT_NOPAGE)))
		return ret;

	/*
	 * For consistency in subsequent calls, make the faulted page always
	 * locked.
	 */
	if (unlikely(!(ret & VM_FAULT_LOCKED)))
		lock_page(vmf.page);
	else
		VM_BUG_ON(!PageLocked(vmf.page));

	/*
	 * Should we do an early C-O-W break?
	 */
	page = vmf.page;
	if (flags & FAULT_FLAG_WRITE) {
		if (!(vma->vm_flags & VM_SHARED)) {
			anon = 1;
			if (unlikely(anon_vma_prepare(vma))) {
				ret = VM_FAULT_OOM;
				goto out;
			}
			page = alloc_page_vma(GFP_HIGHUSER_MOVABLE,
						vma, address);
			if (!page) {
				ret = VM_FAULT_OOM;
				goto out;
			}
			copy_user_highpage(page, vmf.page, address, vma);
			__SetPageUptodate(page);
		} else {
			/*
			 * If the page will be shareable, see if the backing
			 * address space wants to know that the page is about
			 * to become writable
			 */
			if (vma->vm_ops->page_mkwrite) {
				unlock_page(page);
				if (vma->vm_ops->page_mkwrite(vma, page) < 0) {
					ret = VM_FAULT_SIGBUS;
					anon = 1; /* no anon but release vmf.page */
					goto out_unlocked;
				}
				lock_page(page);
				/*
				 * XXX: this is not quite right (racy vs
				 * invalidate) to unlock and relock the page
				 * like this, however a better fix requires
				 * reworking page_mkwrite locking API, which
				 * is better done later.
				 */
				if (!page->mapping) {
					ret = 0;
					anon = 1; /* no anon but release vmf.page */
					goto out;
				}
				page_mkwrite = 1;
			}
		}

	}

	if (mem_cgroup_charge(page, mm, GFP_KERNEL)) {
		ret = VM_FAULT_OOM;
		goto out;
	}

	page_table = pte_offset_map_lock(mm, pmd, address, &ptl);

	/*
	 * This silly early PAGE_DIRTY setting removes a race
	 * due to the bad i386 page protection. But it's valid
	 * for other architectures too.
	 *
	 * Note that if write_access is true, we either now have
	 * an exclusive copy of the page, or this is a shared mapping,
	 * so we can make it writable and dirty to avoid having to
	 * handle that later.
	 */
	/* Only go through if we didn't race with anybody else... */
	if (likely(pte_same(*page_table, orig_pte))) {
		flush_icache_page(vma, page);
		entry = mk_pte(page, vma->vm_page_prot);
		if (flags & FAULT_FLAG_WRITE)
			entry = maybe_mkwrite(pte_mkdirty(entry), vma);
		set_pte_at(mm, address, page_table, entry);
		if (anon) {
                        inc_mm_counter(mm, anon_rss);
                        lru_cache_add_active(page);
                        page_add_new_anon_rmap(page, vma, address);
		} else {
			inc_mm_counter(mm, file_rss);
			page_add_file_rmap(page);
			if (flags & FAULT_FLAG_WRITE) {
				dirty_page = page;
				get_page(dirty_page);
			}
		}

		/* no need to invalidate: a not-present page won't be cached */
		update_mmu_cache(vma, address, entry);
	} else {
		mem_cgroup_uncharge_page(page);
		if (anon)
			page_cache_release(page);
		else
			anon = 1; /* no anon but release faulted_page */
	}

	pte_unmap_unlock(page_table, ptl);

out:
	unlock_page(vmf.page);
out_unlocked:
	if (anon)
		page_cache_release(vmf.page);
	else if (dirty_page) {
		if (vma->vm_file)
			file_update_time(vma->vm_file);

		set_page_dirty_balance(dirty_page, page_mkwrite);
		put_page(dirty_page);
	}

	return ret;
}