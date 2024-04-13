addentry(struct magic_set *ms, struct magic_entry *me,
   struct magic_entry **mentry, uint32_t *mentrycount)
{
	size_t i = me->mp->type == FILE_NAME ? 1 : 0;
	if (mentrycount[i] == maxmagic[i]) {
		struct magic_entry *mp;

		maxmagic[i] += ALLOC_INCR;
		if ((mp = CAST(struct magic_entry *,
		    erealloc(mentry[i], sizeof(*mp) * maxmagic[i]))) ==
		    NULL) {
			file_oomem(ms, sizeof(*mp) * maxmagic[i]);
			return -1;
		}
		(void)memset(&mp[mentrycount[i]], 0, sizeof(*mp) *
		    ALLOC_INCR);
		mentry[i] = mp;
	}
	mentry[i][mentrycount[i]++] = *me;
	memset(me, 0, sizeof(*me));
	return 0;
}