int copy_huge_pmd(struct mm_struct *dst_mm, struct mm_struct *src_mm,
		  pmd_t *dst_pmd, pmd_t *src_pmd, unsigned long addr,
		  struct vm_area_struct *vma)
{
	spinlock_t *dst_ptl, *src_ptl;
	struct page *src_page;
	pmd_t pmd;
	pgtable_t pgtable = NULL;
	int ret = -ENOMEM;

	/* Skip if can be re-fill on fault */
	if (!vma_is_anonymous(vma))
		return 0;

	pgtable = pte_alloc_one(dst_mm);
	if (unlikely(!pgtable))
		goto out;

	dst_ptl = pmd_lock(dst_mm, dst_pmd);
	src_ptl = pmd_lockptr(src_mm, src_pmd);
	spin_lock_nested(src_ptl, SINGLE_DEPTH_NESTING);

	ret = -EAGAIN;
	pmd = *src_pmd;

	/*
	 * Make sure the _PAGE_UFFD_WP bit is cleared if the new VMA
	 * does not have the VM_UFFD_WP, which means that the uffd
	 * fork event is not enabled.
	 */
	if (!(vma->vm_flags & VM_UFFD_WP))
		pmd = pmd_clear_uffd_wp(pmd);

#ifdef CONFIG_ARCH_ENABLE_THP_MIGRATION
	if (unlikely(is_swap_pmd(pmd))) {
		swp_entry_t entry = pmd_to_swp_entry(pmd);

		VM_BUG_ON(!is_pmd_migration_entry(pmd));
		if (is_write_migration_entry(entry)) {
			make_migration_entry_read(&entry);
			pmd = swp_entry_to_pmd(entry);
			if (pmd_swp_soft_dirty(*src_pmd))
				pmd = pmd_swp_mksoft_dirty(pmd);
			set_pmd_at(src_mm, addr, src_pmd, pmd);
		}
		add_mm_counter(dst_mm, MM_ANONPAGES, HPAGE_PMD_NR);
		mm_inc_nr_ptes(dst_mm);
		pgtable_trans_huge_deposit(dst_mm, dst_pmd, pgtable);
		set_pmd_at(dst_mm, addr, dst_pmd, pmd);
		ret = 0;
		goto out_unlock;
	}
#endif

	if (unlikely(!pmd_trans_huge(pmd))) {
		pte_free(dst_mm, pgtable);
		goto out_unlock;
	}
	/*
	 * When page table lock is held, the huge zero pmd should not be
	 * under splitting since we don't split the page itself, only pmd to
	 * a page table.
	 */
	if (is_huge_zero_pmd(pmd)) {
		struct page *zero_page;
		/*
		 * get_huge_zero_page() will never allocate a new page here,
		 * since we already have a zero page to copy. It just takes a
		 * reference.
		 */
		zero_page = mm_get_huge_zero_page(dst_mm);
		set_huge_zero_page(pgtable, dst_mm, vma, addr, dst_pmd,
				zero_page);
		ret = 0;
		goto out_unlock;
	}

	src_page = pmd_page(pmd);
	VM_BUG_ON_PAGE(!PageHead(src_page), src_page);
	get_page(src_page);
	page_dup_rmap(src_page, true);
	add_mm_counter(dst_mm, MM_ANONPAGES, HPAGE_PMD_NR);
	mm_inc_nr_ptes(dst_mm);
	pgtable_trans_huge_deposit(dst_mm, dst_pmd, pgtable);

	pmdp_set_wrprotect(src_mm, addr, src_pmd);
	pmd = pmd_mkold(pmd_wrprotect(pmd));
	set_pmd_at(dst_mm, addr, dst_pmd, pmd);

	ret = 0;
out_unlock:
	spin_unlock(src_ptl);
	spin_unlock(dst_ptl);
out:
	return ret;
}