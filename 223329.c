int split_huge_page_to_list(struct page *page, struct list_head *list)
{
	struct page *head = compound_head(page);
	struct pglist_data *pgdata = NODE_DATA(page_to_nid(head));
	struct deferred_split *ds_queue = get_deferred_split_queue(head);
	struct anon_vma *anon_vma = NULL;
	struct address_space *mapping = NULL;
	int count, mapcount, extra_pins, ret;
	bool mlocked;
	unsigned long flags;
	pgoff_t end;

	VM_BUG_ON_PAGE(is_huge_zero_page(head), head);
	VM_BUG_ON_PAGE(!PageLocked(head), head);
	VM_BUG_ON_PAGE(!PageCompound(head), head);

	if (PageWriteback(head))
		return -EBUSY;

	if (PageAnon(head)) {
		/*
		 * The caller does not necessarily hold an mmap_sem that would
		 * prevent the anon_vma disappearing so we first we take a
		 * reference to it and then lock the anon_vma for write. This
		 * is similar to page_lock_anon_vma_read except the write lock
		 * is taken to serialise against parallel split or collapse
		 * operations.
		 */
		anon_vma = page_get_anon_vma(head);
		if (!anon_vma) {
			ret = -EBUSY;
			goto out;
		}
		end = -1;
		mapping = NULL;
		anon_vma_lock_write(anon_vma);
	} else {
		mapping = head->mapping;

		/* Truncated ? */
		if (!mapping) {
			ret = -EBUSY;
			goto out;
		}

		anon_vma = NULL;
		i_mmap_lock_read(mapping);

		/*
		 *__split_huge_page() may need to trim off pages beyond EOF:
		 * but on 32-bit, i_size_read() takes an irq-unsafe seqlock,
		 * which cannot be nested inside the page tree lock. So note
		 * end now: i_size itself may be changed at any moment, but
		 * head page lock is good enough to serialize the trimming.
		 */
		end = DIV_ROUND_UP(i_size_read(mapping->host), PAGE_SIZE);
	}

	/*
	 * Racy check if we can split the page, before unmap_page() will
	 * split PMDs
	 */
	if (!can_split_huge_page(head, &extra_pins)) {
		ret = -EBUSY;
		goto out_unlock;
	}

	mlocked = PageMlocked(head);
	unmap_page(head);
	VM_BUG_ON_PAGE(compound_mapcount(head), head);

	/* Make sure the page is not on per-CPU pagevec as it takes pin */
	if (mlocked)
		lru_add_drain();

	/* prevent PageLRU to go away from under us, and freeze lru stats */
	spin_lock_irqsave(&pgdata->lru_lock, flags);

	if (mapping) {
		XA_STATE(xas, &mapping->i_pages, page_index(head));

		/*
		 * Check if the head page is present in page cache.
		 * We assume all tail are present too, if head is there.
		 */
		xa_lock(&mapping->i_pages);
		if (xas_load(&xas) != head)
			goto fail;
	}

	/* Prevent deferred_split_scan() touching ->_refcount */
	spin_lock(&ds_queue->split_queue_lock);
	count = page_count(head);
	mapcount = total_mapcount(head);
	if (!mapcount && page_ref_freeze(head, 1 + extra_pins)) {
		if (!list_empty(page_deferred_list(head))) {
			ds_queue->split_queue_len--;
			list_del(page_deferred_list(head));
		}
		spin_unlock(&ds_queue->split_queue_lock);
		if (mapping) {
			if (PageSwapBacked(head))
				__dec_node_page_state(head, NR_SHMEM_THPS);
			else
				__dec_node_page_state(head, NR_FILE_THPS);
		}

		__split_huge_page(page, list, end, flags);
		if (PageSwapCache(head)) {
			swp_entry_t entry = { .val = page_private(head) };

			ret = split_swap_cluster(entry);
		} else
			ret = 0;
	} else {
		if (IS_ENABLED(CONFIG_DEBUG_VM) && mapcount) {
			pr_alert("total_mapcount: %u, page_count(): %u\n",
					mapcount, count);
			if (PageTail(page))
				dump_page(head, NULL);
			dump_page(page, "total_mapcount(head) > 0");
			BUG();
		}
		spin_unlock(&ds_queue->split_queue_lock);
fail:		if (mapping)
			xa_unlock(&mapping->i_pages);
		spin_unlock_irqrestore(&pgdata->lru_lock, flags);
		remap_page(head);
		ret = -EBUSY;
	}

out_unlock:
	if (anon_vma) {
		anon_vma_unlock_write(anon_vma);
		put_anon_vma(anon_vma);
	}
	if (mapping)
		i_mmap_unlock_read(mapping);
out:
	count_vm_event(!ret ? THP_SPLIT_PAGE : THP_SPLIT_PAGE_FAILED);
	return ret;
}