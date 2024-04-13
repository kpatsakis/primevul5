static int madvise_inject_error(int behavior,
		unsigned long start, unsigned long end)
{
	struct page *page;
	struct zone *zone;
	unsigned long size;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;


	for (; start < end; start += size) {
		unsigned long pfn;
		int ret;

		ret = get_user_pages_fast(start, 1, 0, &page);
		if (ret != 1)
			return ret;
		pfn = page_to_pfn(page);

		/*
		 * When soft offlining hugepages, after migrating the page
		 * we dissolve it, therefore in the second loop "page" will
		 * no longer be a compound page.
		 */
		size = page_size(compound_head(page));

		if (PageHWPoison(page)) {
			put_page(page);
			continue;
		}

		if (behavior == MADV_SOFT_OFFLINE) {
			pr_info("Soft offlining pfn %#lx at process virtual address %#lx\n",
					pfn, start);

			ret = soft_offline_page(pfn, MF_COUNT_INCREASED);
			if (ret)
				return ret;
			continue;
		}

		pr_info("Injecting memory failure for pfn %#lx at process virtual address %#lx\n",
				pfn, start);

		/*
		 * Drop the page reference taken by get_user_pages_fast(). In
		 * the absence of MF_COUNT_INCREASED the memory_failure()
		 * routine is responsible for pinning the page to prevent it
		 * from being released back to the page allocator.
		 */
		put_page(page);
		ret = memory_failure(pfn, 0);
		if (ret)
			return ret;
	}

	/* Ensure that all poisoned pages are removed from per-cpu lists */
	for_each_populated_zone(zone)
		drain_all_pages(zone);

	return 0;
}