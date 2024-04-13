static int queue_pages_pmd(pmd_t *pmd, spinlock_t *ptl, unsigned long addr,
				unsigned long end, struct mm_walk *walk)
{
	int ret = 0;
	struct page *page;
	struct queue_pages *qp = walk->private;
	unsigned long flags;

	if (unlikely(is_pmd_migration_entry(*pmd))) {
		ret = -EIO;
		goto unlock;
	}
	page = pmd_page(*pmd);
	if (is_huge_zero_page(page)) {
		spin_unlock(ptl);
		__split_huge_pmd(walk->vma, pmd, addr, false, NULL);
		ret = 2;
		goto out;
	}
	if (!queue_pages_required(page, qp))
		goto unlock;

	flags = qp->flags;
	/* go to thp migration */
	if (flags & (MPOL_MF_MOVE | MPOL_MF_MOVE_ALL)) {
		if (!vma_migratable(walk->vma) ||
		    migrate_page_add(page, qp->pagelist, flags)) {
			ret = 1;
			goto unlock;
		}
	} else
		ret = -EIO;
unlock:
	spin_unlock(ptl);
out:
	return ret;
}