static int do_move_pages(struct mm_struct *mm, struct page_to_node *pm,
				int migrate_all)
{
	int err;
	struct page_to_node *pp;
	LIST_HEAD(pagelist);

	down_read(&mm->mmap_sem);

	/*
	 * Build a list of pages to migrate
	 */
	migrate_prep();
	for (pp = pm; pp->node != MAX_NUMNODES; pp++) {
		struct vm_area_struct *vma;
		struct page *page;

		/*
		 * A valid page pointer that will not match any of the
		 * pages that will be moved.
		 */
		pp->page = ZERO_PAGE(0);

		err = -EFAULT;
		vma = find_vma(mm, pp->addr);
		if (!vma || !vma_migratable(vma))
			goto set_status;

		page = follow_page(vma, pp->addr, FOLL_GET);

		err = PTR_ERR(page);
		if (IS_ERR(page))
			goto set_status;

		err = -ENOENT;
		if (!page)
			goto set_status;

		if (PageReserved(page))		/* Check for zero page */
			goto put_and_set;

		pp->page = page;
		err = page_to_nid(page);

		if (err == pp->node)
			/*
			 * Node already in the right place
			 */
			goto put_and_set;

		err = -EACCES;
		if (page_mapcount(page) > 1 &&
				!migrate_all)
			goto put_and_set;

		err = isolate_lru_page(page, &pagelist);
put_and_set:
		/*
		 * Either remove the duplicate refcount from
		 * isolate_lru_page() or drop the page ref if it was
		 * not isolated.
		 */
		put_page(page);
set_status:
		pp->status = err;
	}

	if (!list_empty(&pagelist))
		err = migrate_pages(&pagelist, new_page_node,
				(unsigned long)pm);
	else
		err = -ENOENT;

	up_read(&mm->mmap_sem);
	return err;
}