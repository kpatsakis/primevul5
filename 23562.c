static int unmap_and_move(new_page_t get_new_page, unsigned long private,
			struct page *page, int force)
{
	int rc = 0;
	int *result = NULL;
	struct page *newpage = get_new_page(page, private, &result);
	int rcu_locked = 0;
	int charge = 0;

	if (!newpage)
		return -ENOMEM;

	if (page_count(page) == 1)
		/* page was freed from under us. So we are done. */
		goto move_newpage;

	rc = -EAGAIN;
	if (TestSetPageLocked(page)) {
		if (!force)
			goto move_newpage;
		lock_page(page);
	}

	if (PageWriteback(page)) {
		if (!force)
			goto unlock;
		wait_on_page_writeback(page);
	}
	/*
	 * By try_to_unmap(), page->mapcount goes down to 0 here. In this case,
	 * we cannot notice that anon_vma is freed while we migrates a page.
	 * This rcu_read_lock() delays freeing anon_vma pointer until the end
	 * of migration. File cache pages are no problem because of page_lock()
	 * File Caches may use write_page() or lock_page() in migration, then,
	 * just care Anon page here.
	 */
	if (PageAnon(page)) {
		rcu_read_lock();
		rcu_locked = 1;
	}

	/*
	 * Corner case handling:
	 * 1. When a new swap-cache page is read into, it is added to the LRU
	 * and treated as swapcache but it has no rmap yet.
	 * Calling try_to_unmap() against a page->mapping==NULL page will
	 * trigger a BUG.  So handle it here.
	 * 2. An orphaned page (see truncate_complete_page) might have
	 * fs-private metadata. The page can be picked up due to memory
	 * offlining.  Everywhere else except page reclaim, the page is
	 * invisible to the vm, so the page can not be migrated.  So try to
	 * free the metadata, so the page can be freed.
	 */
	if (!page->mapping) {
		if (!PageAnon(page) && PagePrivate(page)) {
			/*
			 * Go direct to try_to_free_buffers() here because
			 * a) that's what try_to_release_page() would do anyway
			 * b) we may be under rcu_read_lock() here, so we can't
			 *    use GFP_KERNEL which is what try_to_release_page()
			 *    needs to be effective.
			 */
			try_to_free_buffers(page);
		}
		goto rcu_unlock;
	}

	charge = mem_cgroup_prepare_migration(page);
	/* Establish migration ptes or remove ptes */
	try_to_unmap(page, 1);

	if (!page_mapped(page))
		rc = move_to_new_page(newpage, page);

	if (rc) {
		remove_migration_ptes(page, page);
		if (charge)
			mem_cgroup_end_migration(page);
	} else if (charge)
 		mem_cgroup_end_migration(newpage);
rcu_unlock:
	if (rcu_locked)
		rcu_read_unlock();

unlock:

	unlock_page(page);

	if (rc != -EAGAIN) {
 		/*
 		 * A page that has been migrated has all references
 		 * removed and will be freed. A page that has not been
 		 * migrated will have kepts its references and be
 		 * restored.
 		 */
 		list_del(&page->lru);
 		move_to_lru(page);
	}

move_newpage:
	/*
	 * Move the new page to the LRU. If migration was not successful
	 * then this will free the page.
	 */
	move_to_lru(newpage);
	if (result) {
		if (rc)
			*result = rc;
		else
			*result = page_to_nid(newpage);
	}
	return rc;
}