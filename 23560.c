int migrate_pages(struct list_head *from,
		new_page_t get_new_page, unsigned long private)
{
	int retry = 1;
	int nr_failed = 0;
	int pass = 0;
	struct page *page;
	struct page *page2;
	int swapwrite = current->flags & PF_SWAPWRITE;
	int rc;

	if (!swapwrite)
		current->flags |= PF_SWAPWRITE;

	for(pass = 0; pass < 10 && retry; pass++) {
		retry = 0;

		list_for_each_entry_safe(page, page2, from, lru) {
			cond_resched();

			rc = unmap_and_move(get_new_page, private,
						page, pass > 2);

			switch(rc) {
			case -ENOMEM:
				goto out;
			case -EAGAIN:
				retry++;
				break;
			case 0:
				break;
			default:
				/* Permanent failure */
				nr_failed++;
				break;
			}
		}
	}
	rc = 0;
out:
	if (!swapwrite)
		current->flags &= ~PF_SWAPWRITE;

	putback_lru_pages(from);

	if (rc)
		return rc;

	return nr_failed + retry;
}