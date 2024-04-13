mlist_alloc(void)
{
	struct mlist *mlist;
	if ((mlist = CAST(struct mlist *, ecalloc(1, sizeof(*mlist)))) == NULL) {
		return NULL;
	}
	mlist->next = mlist->prev = mlist;
	return mlist;
}