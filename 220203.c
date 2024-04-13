static void force_shm_swapin_readahead(struct vm_area_struct *vma,
		unsigned long start, unsigned long end,
		struct address_space *mapping)
{
	pgoff_t index;
	struct page *page;
	swp_entry_t swap;

	for (; start < end; start += PAGE_SIZE) {
		index = ((start - vma->vm_start) >> PAGE_SHIFT) + vma->vm_pgoff;

		page = find_get_entry(mapping, index);
		if (!xa_is_value(page)) {
			if (page)
				put_page(page);
			continue;
		}
		swap = radix_to_swp_entry(page);
		page = read_swap_cache_async(swap, GFP_HIGHUSER_MOVABLE,
							NULL, 0, false);
		if (page)
			put_page(page);
	}

	lru_add_drain();	/* Push any new pages onto the LRU now */
}