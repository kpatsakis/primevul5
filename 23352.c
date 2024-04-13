static void xennet_end_access(int ref, void *page)
{
	/* This frees the page as a side-effect */
	if (ref != INVALID_GRANT_REF)
		gnttab_end_foreign_access(ref, virt_to_page(page));
}