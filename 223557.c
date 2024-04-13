static int migrate_to_node(struct mm_struct *mm, int source, int dest,
			   int flags)
{
	nodemask_t nmask;
	LIST_HEAD(pagelist);
	int err = 0;

	nodes_clear(nmask);
	node_set(source, nmask);

	/*
	 * This does not "check" the range but isolates all pages that
	 * need migration.  Between passing in the full user address
	 * space range and MPOL_MF_DISCONTIG_OK, this call can not fail.
	 */
	VM_BUG_ON(!(flags & (MPOL_MF_MOVE | MPOL_MF_MOVE_ALL)));
	queue_pages_range(mm, mm->mmap->vm_start, mm->task_size, &nmask,
			flags | MPOL_MF_DISCONTIG_OK, &pagelist);

	if (!list_empty(&pagelist)) {
		err = migrate_pages(&pagelist, alloc_new_node_page, NULL, dest,
					MIGRATE_SYNC, MR_SYSCALL);
		if (err)
			putback_movable_pages(&pagelist);
	}

	return err;
}