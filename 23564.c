static int do_wp_page(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, pte_t *page_table, pmd_t *pmd,
		spinlock_t *ptl, pte_t orig_pte)
{
	struct page *old_page, *new_page;
	pte_t entry;
	int reuse = 0, ret = 0;
	int page_mkwrite = 0;
	struct page *dirty_page = NULL;

	old_page = vm_normal_page(vma, address, orig_pte);
	if (!old_page)
		goto gotten;

	/*
	 * Take out anonymous pages first, anonymous shared vmas are
	 * not dirty accountable.
	 */
	if (PageAnon(old_page)) {
		if (!TestSetPageLocked(old_page)) {
			reuse = can_share_swap_page(old_page);
			unlock_page(old_page);
		}
	} else if (unlikely((vma->vm_flags & (VM_WRITE|VM_SHARED)) ==
					(VM_WRITE|VM_SHARED))) {
		/*
		 * Only catch write-faults on shared writable pages,
		 * read-only shared pages can get COWed by
		 * get_user_pages(.write=1, .force=1).
		 */
		if (vma->vm_ops && vma->vm_ops->page_mkwrite) {
			/*
			 * Notify the address space that the page is about to
			 * become writable so that it can prohibit this or wait
			 * for the page to get into an appropriate state.
			 *
			 * We do this without the lock held, so that it can
			 * sleep if it needs to.
			 */
			page_cache_get(old_page);
			pte_unmap_unlock(page_table, ptl);

			if (vma->vm_ops->page_mkwrite(vma, old_page) < 0)
				goto unwritable_page;

			/*
			 * Since we dropped the lock we need to revalidate
			 * the PTE as someone else may have changed it.  If
			 * they did, we just return, as we can count on the
			 * MMU to tell us if they didn't also make it writable.
			 */
			page_table = pte_offset_map_lock(mm, pmd, address,
							 &ptl);
			page_cache_release(old_page);
			if (!pte_same(*page_table, orig_pte))
				goto unlock;

			page_mkwrite = 1;
		}
		dirty_page = old_page;
		get_page(dirty_page);
		reuse = 1;
	}

	if (reuse) {
		flush_cache_page(vma, address, pte_pfn(orig_pte));
		entry = pte_mkyoung(orig_pte);
		entry = maybe_mkwrite(pte_mkdirty(entry), vma);
		if (ptep_set_access_flags(vma, address, page_table, entry,1))
			update_mmu_cache(vma, address, entry);
		ret |= VM_FAULT_WRITE;
		goto unlock;
	}

	/*
	 * Ok, we need to copy. Oh, well..
	 */
	page_cache_get(old_page);
gotten:
	pte_unmap_unlock(page_table, ptl);

	if (unlikely(anon_vma_prepare(vma)))
		goto oom;
	VM_BUG_ON(old_page == ZERO_PAGE(0));
	new_page = alloc_page_vma(GFP_HIGHUSER_MOVABLE, vma, address);
	if (!new_page)
		goto oom;
	cow_user_page(new_page, old_page, address, vma);
	__SetPageUptodate(new_page);

	if (mem_cgroup_charge(new_page, mm, GFP_KERNEL))
		goto oom_free_new;

	/*
	 * Re-check the pte - we dropped the lock
	 */
	page_table = pte_offset_map_lock(mm, pmd, address, &ptl);
	if (likely(pte_same(*page_table, orig_pte))) {
		if (old_page) {
			page_remove_rmap(old_page, vma);
			if (!PageAnon(old_page)) {
				dec_mm_counter(mm, file_rss);
				inc_mm_counter(mm, anon_rss);
			}
		} else
			inc_mm_counter(mm, anon_rss);
		flush_cache_page(vma, address, pte_pfn(orig_pte));
		entry = mk_pte(new_page, vma->vm_page_prot);
		entry = maybe_mkwrite(pte_mkdirty(entry), vma);
		/*
		 * Clear the pte entry and flush it first, before updating the
		 * pte with the new entry. This will avoid a race condition
		 * seen in the presence of one thread doing SMC and another
		 * thread doing COW.
		 */
		ptep_clear_flush(vma, address, page_table);
		set_pte_at(mm, address, page_table, entry);
		update_mmu_cache(vma, address, entry);
		lru_cache_add_active(new_page);
		page_add_new_anon_rmap(new_page, vma, address);

		/* Free the old page.. */
		new_page = old_page;
		ret |= VM_FAULT_WRITE;
	} else
		mem_cgroup_uncharge_page(new_page);

	if (new_page)
		page_cache_release(new_page);
	if (old_page)
		page_cache_release(old_page);
unlock:
	pte_unmap_unlock(page_table, ptl);
	if (dirty_page) {
		if (vma->vm_file)
			file_update_time(vma->vm_file);

		/*
		 * Yes, Virginia, this is actually required to prevent a race
		 * with clear_page_dirty_for_io() from clearing the page dirty
		 * bit after it clear all dirty ptes, but before a racing
		 * do_wp_page installs a dirty pte.
		 *
		 * do_no_page is protected similarly.
		 */
		wait_on_page_locked(dirty_page);
		set_page_dirty_balance(dirty_page, page_mkwrite);
		put_page(dirty_page);
	}
	return ret;
oom_free_new:
	page_cache_release(new_page);
oom:
	if (old_page)
		page_cache_release(old_page);
	return VM_FAULT_OOM;

unwritable_page:
	page_cache_release(old_page);
	return VM_FAULT_SIGBUS;
}