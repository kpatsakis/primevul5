parse_mime(struct magic_set *ms, struct magic_entry *me, const char *line)
{
	size_t i;
	const char *l = line;
	struct magic *m = &me->mp[me->cont_count == 0 ? 0 : me->cont_count - 1];

	if (m->mimetype[0] != '\0') {
		file_magwarn(ms, "Current entry already has a MIME type `%s',"
		    " new type `%s'", m->mimetype, l);
		return -1;
	}	

	EATAB;
	for (i = 0; *l && ((isascii((unsigned char)*l) &&
	    isalnum((unsigned char)*l)) || strchr("-+/.", *l)) &&
	    i < sizeof(m->mimetype); m->mimetype[i++] = *l++)
		continue;
	if (i == sizeof(m->mimetype)) {
		m->mimetype[sizeof(m->mimetype) - 1] = '\0';
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "MIME type `%s' truncated %"
			    SIZE_T_FORMAT "u", m->mimetype, i);
	} else
		m->mimetype[i] = '\0';

	if (i > 0)
		return 0;
	else
		return -1;
}