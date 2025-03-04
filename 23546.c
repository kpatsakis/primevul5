static int do_pages_stat(struct mm_struct *mm, struct page_to_node *pm)
{
	down_read(&mm->mmap_sem);

	for ( ; pm->node != MAX_NUMNODES; pm++) {
		struct vm_area_struct *vma;
		struct page *page;
		int err;

		err = -EFAULT;
		vma = find_vma(mm, pm->addr);
		if (!vma)
			goto set_status;

		page = follow_page(vma, pm->addr, 0);

		err = PTR_ERR(page);
		if (IS_ERR(page))
			goto set_status;

		err = -ENOENT;
		/* Use PageReserved to check for zero page */
		if (!page || PageReserved(page))
			goto set_status;

		err = page_to_nid(page);
set_status:
		pm->status = err;
	}

	up_read(&mm->mmap_sem);
	return 0;
}