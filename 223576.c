static int queue_pages_hugetlb(pte_t *pte, unsigned long hmask,
			       unsigned long addr, unsigned long end,
			       struct mm_walk *walk)
{
	int ret = 0;
#ifdef CONFIG_HUGETLB_PAGE
	struct queue_pages *qp = walk->private;
	unsigned long flags = (qp->flags & MPOL_MF_VALID);
	struct page *page;
	spinlock_t *ptl;
	pte_t entry;

	ptl = huge_pte_lock(hstate_vma(walk->vma), walk->mm, pte);
	entry = huge_ptep_get(pte);
	if (!pte_present(entry))
		goto unlock;
	page = pte_page(entry);
	if (!queue_pages_required(page, qp))
		goto unlock;

	if (flags == MPOL_MF_STRICT) {
		/*
		 * STRICT alone means only detecting misplaced page and no
		 * need to further check other vma.
		 */
		ret = -EIO;
		goto unlock;
	}

	if (!vma_migratable(walk->vma)) {
		/*
		 * Must be STRICT with MOVE*, otherwise .test_walk() have
		 * stopped walking current vma.
		 * Detecting misplaced page but allow migrating pages which
		 * have been queued.
		 */
		ret = 1;
		goto unlock;
	}

	/* With MPOL_MF_MOVE, we migrate only unshared hugepage. */
	if (flags & (MPOL_MF_MOVE_ALL) ||
	    (flags & MPOL_MF_MOVE && page_mapcount(page) == 1)) {
		if (!isolate_huge_page(page, qp->pagelist) &&
			(flags & MPOL_MF_STRICT))
			/*
			 * Failed to isolate page but allow migrating pages
			 * which have been queued.
			 */
			ret = 1;
	}
unlock:
	spin_unlock(ptl);
#else
	BUG();
#endif
	return ret;
}