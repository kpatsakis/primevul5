file_ms_free(struct magic_set *ms)
{
	size_t i;
	if (ms == NULL)
		return;
	for (i = 0; i < MAGIC_SETS; i++)
		mlist_free(ms->mlist[i]);
	if (ms->o.pbuf) {
		efree(ms->o.pbuf);
	}
	if (ms->o.buf) {
		efree(ms->o.buf);
	}
	if (ms->c.li) {
		efree(ms->c.li);
	}
	efree(ms);
}