parse_apple(struct magic_set *ms, struct magic_entry *me, const char *line)
{
	size_t i;
	const char *l = line;
	struct magic *m = &me->mp[me->cont_count == 0 ? 0 : me->cont_count - 1];

	if (m->apple[0] != '\0') {
		file_magwarn(ms, "Current entry already has a APPLE type "
		    "`%.8s', new type `%s'", m->mimetype, l);
		return -1;
	}	

	EATAB;
	for (i = 0; *l && ((isascii((unsigned char)*l) &&
	    isalnum((unsigned char)*l)) || strchr("-+/.", *l)) &&
	    i < sizeof(m->apple); m->apple[i++] = *l++)
		continue;
	if (i == sizeof(m->apple) && *l) {
		/* We don't need to NUL terminate here, printing handles it */
		if (ms->flags & MAGIC_CHECK)
			file_magwarn(ms, "APPLE type `%s' truncated %"
			    SIZE_T_FORMAT "u", line, i);
	}

	if (i > 0)
		return 0;
	else
		return -1;
}