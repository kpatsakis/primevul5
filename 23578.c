static int do_anonymous_page(struct mm_struct *mm, struct vm_area_struct *vma,
		unsigned long address, pte_t *page_table, pmd_t *pmd,
		int write_access)
{
	struct page *page;
	spinlock_t *ptl;
	pte_t entry;

	/* Allocate our own private page. */
	pte_unmap(page_table);

	if (unlikely(anon_vma_prepare(vma)))
		goto oom;
	page = alloc_zeroed_user_highpage_movable(vma, address);
	if (!page)
		goto oom;
	__SetPageUptodate(page);

	if (mem_cgroup_charge(page, mm, GFP_KERNEL))
		goto oom_free_page;

	entry = mk_pte(page, vma->vm_page_prot);
	entry = maybe_mkwrite(pte_mkdirty(entry), vma);

	page_table = pte_offset_map_lock(mm, pmd, address, &ptl);
	if (!pte_none(*page_table))
		goto release;
	inc_mm_counter(mm, anon_rss);
	lru_cache_add_active(page);
	page_add_new_anon_rmap(page, vma, address);
	set_pte_at(mm, address, page_table, entry);

	/* No need to invalidate - it was non-present before */
	update_mmu_cache(vma, address, entry);
unlock:
	pte_unmap_unlock(page_table, ptl);
	return 0;
release:
	mem_cgroup_uncharge_page(page);
	page_cache_release(page);
	goto unlock;
oom_free_page:
	page_cache_release(page);
oom:
	return VM_FAULT_OOM;
}