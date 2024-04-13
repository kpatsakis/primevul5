static int do_swap_page(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, pte_t *page_table, pmd_t *pmd,
		int write_access, pte_t orig_pte)
{
	spinlock_t *ptl;
	struct page *page;
	swp_entry_t entry;
	pte_t pte;
	int ret = 0;

	if (!pte_unmap_same(mm, pmd, page_table, orig_pte))
		goto out;

	entry = pte_to_swp_entry(orig_pte);
	if (is_migration_entry(entry)) {
		migration_entry_wait(mm, pmd, address);
		goto out;
	}
	delayacct_set_flag(DELAYACCT_PF_SWAPIN);
	page = lookup_swap_cache(entry);
	if (!page) {
		grab_swap_token(); /* Contend for token _before_ read-in */
		page = swapin_readahead(entry,
					GFP_HIGHUSER_MOVABLE, vma, address);
		if (!page) {
			/*
			 * Back out if somebody else faulted in this pte
			 * while we released the pte lock.
			 */
			page_table = pte_offset_map_lock(mm, pmd, address, &ptl);
			if (likely(pte_same(*page_table, orig_pte)))
				ret = VM_FAULT_OOM;
			delayacct_clear_flag(DELAYACCT_PF_SWAPIN);
			goto unlock;
		}

		/* Had to read the page from swap area: Major fault */
		ret = VM_FAULT_MAJOR;
		count_vm_event(PGMAJFAULT);
	}

	if (mem_cgroup_charge(page, mm, GFP_KERNEL)) {
		delayacct_clear_flag(DELAYACCT_PF_SWAPIN);
		ret = VM_FAULT_OOM;
		goto out;
	}

	mark_page_accessed(page);
	lock_page(page);
	delayacct_clear_flag(DELAYACCT_PF_SWAPIN);

	/*
	 * Back out if somebody else already faulted in this pte.
	 */
	page_table = pte_offset_map_lock(mm, pmd, address, &ptl);
	if (unlikely(!pte_same(*page_table, orig_pte)))
		goto out_nomap;

	if (unlikely(!PageUptodate(page))) {
		ret = VM_FAULT_SIGBUS;
		goto out_nomap;
	}

	/* The page isn't present yet, go ahead with the fault. */

	inc_mm_counter(mm, anon_rss);
	pte = mk_pte(page, vma->vm_page_prot);
	if (write_access && can_share_swap_page(page)) {
		pte = maybe_mkwrite(pte_mkdirty(pte), vma);
		write_access = 0;
	}

	flush_icache_page(vma, page);
	set_pte_at(mm, address, page_table, pte);
	page_add_anon_rmap(page, vma, address);

	swap_free(entry);
	if (vm_swap_full())
		remove_exclusive_swap_page(page);
	unlock_page(page);

	if (write_access) {
		ret |= do_wp_page(mm, vma, address, page_table, pmd, ptl, pte);
		if (ret & VM_FAULT_ERROR)
			ret &= VM_FAULT_ERROR;
		goto out;
	}

	/* No need to invalidate - it was non-present before */
	update_mmu_cache(vma, address, pte);
unlock:
	pte_unmap_unlock(page_table, ptl);
out:
	return ret;
out_nomap:
	mem_cgroup_uncharge_page(page);
	pte_unmap_unlock(page_table, ptl);
	unlock_page(page);
	page_cache_release(page);
	return ret;
}