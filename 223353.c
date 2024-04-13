static int split_huge_pages_set(void *data, u64 val)
{
	struct zone *zone;
	struct page *page;
	unsigned long pfn, max_zone_pfn;
	unsigned long total = 0, split = 0;

	if (val != 1)
		return -EINVAL;

	for_each_populated_zone(zone) {
		max_zone_pfn = zone_end_pfn(zone);
		for (pfn = zone->zone_start_pfn; pfn < max_zone_pfn; pfn++) {
			if (!pfn_valid(pfn))
				continue;

			page = pfn_to_page(pfn);
			if (!get_page_unless_zero(page))
				continue;

			if (zone != page_zone(page))
				goto next;

			if (!PageHead(page) || PageHuge(page) || !PageLRU(page))
				goto next;

			total++;
			lock_page(page);
			if (!split_huge_page(page))
				split++;
			unlock_page(page);
next:
			put_page(page);
		}
	}

	pr_info("%lu of %lu THP split\n", split, total);

	return 0;
}