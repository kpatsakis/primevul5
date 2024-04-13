apprentice_1(struct magic_set *ms, const char *fn, int action)
{
	struct mlist *ml;
	struct magic_map *map;
	size_t i;

	if (magicsize != FILE_MAGICSIZE) {
		file_error(ms, 0, "magic element size %lu != %lu",
		    (unsigned long)sizeof(*map->magic[0]),
		    (unsigned long)FILE_MAGICSIZE);
		return -1;
	}

	if (action == FILE_COMPILE) {
		map = apprentice_load(ms, fn, action);
		if (map == NULL)
			return -1;
		return apprentice_compile(ms, map, fn);
	}

	map = apprentice_map(ms, fn);
	if (map == NULL) {
		if (fn) {
			if (ms->flags & MAGIC_CHECK)
				file_magwarn(ms, "using regular magic file `%s'", fn);
			map = apprentice_load(ms, fn, action);
		}
		if (map == NULL)
			return -1;
	}

	for (i = 0; i < MAGIC_SETS; i++) {
		if (add_mlist(ms->mlist[i], map, i) == -1) {
			file_oomem(ms, sizeof(*ml));
			apprentice_unmap(map);
			return -1;
		}
	}

	if (action == FILE_LIST) {
		for (i = 0; i < MAGIC_SETS; i++) {
			printf("Set %zu:\nBinary patterns:\n", i);
			apprentice_list(ms->mlist[i], BINTEST);
			printf("Text patterns:\n");
			apprentice_list(ms->mlist[i], TEXTTEST);
		}
	}
	
	return 0;
}