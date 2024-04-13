file_ms_alloc(int flags)
{
	struct magic_set *ms;
	size_t i, len;

	if ((ms = CAST(struct magic_set *, ecalloc((size_t)1,
	    sizeof(struct magic_set)))) == NULL)
		return NULL;

	if (magic_setflags(ms, flags) == -1) {
		errno = EINVAL;
		goto free;
	}

	ms->o.buf = ms->o.pbuf = NULL;
	len = (ms->c.len = 10) * sizeof(*ms->c.li);

	if ((ms->c.li = CAST(struct level_info *, emalloc(len))) == NULL)
		goto free;

	ms->event_flags = 0;
	ms->error = -1;
	for (i = 0; i < MAGIC_SETS; i++)
		ms->mlist[i] = NULL;
	ms->file = "unknown";
	ms->line = 0;
	return ms;
free:
	efree(ms);
	return NULL;
}