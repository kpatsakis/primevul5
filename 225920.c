coalesce_entries(struct magic_set *ms, struct magic_entry *me, uint32_t nme,
    struct magic **ma, uint32_t *nma)
{
	uint32_t i, mentrycount = 0;
	size_t slen;

	for (i = 0; i < nme; i++)
		mentrycount += me[i].cont_count;

	slen = sizeof(**ma) * mentrycount;
	if ((*ma = CAST(struct magic *, emalloc(slen))) == NULL) {
		file_oomem(ms, slen);
		return -1;
	}

	mentrycount = 0;
	for (i = 0; i < nme; i++) {
		(void)memcpy(*ma + mentrycount, me[i].mp,
		    me[i].cont_count * sizeof(**ma));
		mentrycount += me[i].cont_count;
	}
	*nma = mentrycount;
	return 0;
}