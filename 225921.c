file_apprentice(struct magic_set *ms, const char *fn, int action)
{
	char *p, *mfn;
	int file_err, errs = -1;
	size_t i;
/* XXX disabling default magic loading so the compiled in data is used */
#if 0
	if ((fn = magic_getpath(fn, action)) == NULL)
		return -1;
#endif

	init_file_tables();

	if (fn == NULL)
		fn = getenv("MAGIC");
	if (fn == NULL) {
		for (i = 0; i < MAGIC_SETS; i++) {
			mlist_free(ms->mlist[i]);
			if ((ms->mlist[i] = mlist_alloc()) == NULL) {
				file_oomem(ms, sizeof(*ms->mlist[i]));
				return -1;
			}
		}
		return apprentice_1(ms, fn, action);
	}

	if ((mfn = estrdup(fn)) == NULL) {
		file_oomem(ms, strlen(fn));
		return -1;
	}

	for (i = 0; i < MAGIC_SETS; i++) {
		mlist_free(ms->mlist[i]);
		if ((ms->mlist[i] = mlist_alloc()) == NULL) {
			file_oomem(ms, sizeof(*ms->mlist[i]));
			if (i != 0) {
				--i;
				do
					mlist_free(ms->mlist[i]);
				while (i != 0);
			}
			efree(mfn);
			return -1;
		}
	}
	fn = mfn;

	while (fn) {
		p = strchr(fn, PATHSEP);
		if (p)
			*p++ = '\0';
		if (*fn == '\0')
			break;
		file_err = apprentice_1(ms, fn, action);
		errs = MAX(errs, file_err);
		fn = p;
	}

	efree(mfn);

	if (errs == -1) {
		for (i = 0; i < MAGIC_SETS; i++) {
			mlist_free(ms->mlist[i]);
			ms->mlist[i] = NULL;
		}
		file_error(ms, 0, "could not find any valid magic files!");
		return -1;
	}

	if (action == FILE_LOAD)
		return 0;

	for (i = 0; i < MAGIC_SETS; i++) {
		mlist_free(ms->mlist[i]);
		ms->mlist[i] = NULL;
	}

	switch (action) {
	case FILE_COMPILE:
	case FILE_CHECK:
	case FILE_LIST:
		return 0;
	default:
		file_error(ms, 0, "Invalid action %d", action);
		return -1;
	}
}