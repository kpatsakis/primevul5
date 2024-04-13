struct file_list *flist_for_ndx(int ndx, const char *fatal_error_loc)
{
	struct file_list *flist = cur_flist;

	if (!flist && !(flist = first_flist))
		goto not_found;

	while (ndx < flist->ndx_start-1) {
		if (flist == first_flist)
			goto not_found;
		flist = flist->prev;
	}
	while (ndx >= flist->ndx_start + flist->used) {
		if (!(flist = flist->next))
			goto not_found;
	}
	return flist;

  not_found:
	if (fatal_error_loc) {
		int first, last;
		if (first_flist) {
			first = first_flist->ndx_start - 1;
			last = first_flist->prev->ndx_start + first_flist->prev->used - 1;
		} else {
			first = 0;
			last = -1;
		}
		rprintf(FERROR,
			"File-list index %d not in %d - %d (%s) [%s]\n",
			ndx, first, last, fatal_error_loc, who_am_i());
		exit_cleanup(RERR_PROTOCOL);
	}
	return NULL;
}
