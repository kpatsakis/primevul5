static int lookup_node(struct mm_struct *mm, unsigned long addr)
{
	struct page *p;
	int err;

	int locked = 1;
	err = get_user_pages_locked(addr & PAGE_MASK, 1, 0, &p, &locked);
	if (err >= 0) {
		err = page_to_nid(p);
		put_page(p);
	}
	if (locked)
		up_read(&mm->mmap_sem);
	return err;
}