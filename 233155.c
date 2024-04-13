write_vim_spell(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    int		regionmask;
    int		round;
    wordnode_T	*tree;
    int		nodecount;
    int		i;
    int		l;
    garray_T	*gap;
    fromto_T	*ftp;
    char_u	*p;
    int		rr;
    int		retval = OK;
    size_t	fwv = 1;  /* collect return value of fwrite() to avoid
			     warnings from picky compiler */

    fd = mch_fopen((char *)fname, "w");
    if (fd == NULL)
    {
	EMSG2(_(e_notopen), fname);
	return FAIL;
    }

    /* <HEADER>: <fileID> <versionnr> */
							    /* <fileID> */
    fwv &= fwrite(VIMSPELLMAGIC, VIMSPELLMAGICL, (size_t)1, fd);
    if (fwv != (size_t)1)
	/* Catch first write error, don't try writing more. */
	goto theend;

    putc(VIMSPELLVERSION, fd);				    /* <versionnr> */

    /*
     * <SECTIONS>: <section> ... <sectionend>
     */

    /* SN_INFO: <infotext> */
    if (spin->si_info != NULL)
    {
	putc(SN_INFO, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */

	i = (int)STRLEN(spin->si_info);
	put_bytes(fd, (long_u)i, 4);			/* <sectionlen> */
	fwv &= fwrite(spin->si_info, (size_t)i, (size_t)1, fd); /* <infotext> */
    }

    /* SN_REGION: <regionname> ...
     * Write the region names only if there is more than one. */
    if (spin->si_region_count > 1)
    {
	putc(SN_REGION, fd);				/* <sectionID> */
	putc(SNF_REQUIRED, fd);				/* <sectionflags> */
	l = spin->si_region_count * 2;
	put_bytes(fd, (long_u)l, 4);			/* <sectionlen> */
	fwv &= fwrite(spin->si_region_name, (size_t)l, (size_t)1, fd);
							/* <regionname> ... */
	regionmask = (1 << spin->si_region_count) - 1;
    }
    else
	regionmask = 0;

    /* SN_CHARFLAGS: <charflagslen> <charflags> <folcharslen> <folchars>
     *
     * The table with character flags and the table for case folding.
     * This makes sure the same characters are recognized as word characters
     * when generating an when using a spell file.
     * Skip this for ASCII, the table may conflict with the one used for
     * 'encoding'.
     * Also skip this for an .add.spl file, the main spell file must contain
     * the table (avoids that it conflicts).  File is shorter too.
     */
    if (!spin->si_ascii && !spin->si_add)
    {
	char_u	folchars[128 * 8];
	int	flags;

	putc(SN_CHARFLAGS, fd);				/* <sectionID> */
	putc(SNF_REQUIRED, fd);				/* <sectionflags> */

	/* Form the <folchars> string first, we need to know its length. */
	l = 0;
	for (i = 128; i < 256; ++i)
	{
#ifdef FEAT_MBYTE
	    if (has_mbyte)
		l += mb_char2bytes(spelltab.st_fold[i], folchars + l);
	    else
#endif
		folchars[l++] = spelltab.st_fold[i];
	}
	put_bytes(fd, (long_u)(1 + 128 + 2 + l), 4);	/* <sectionlen> */

	fputc(128, fd);					/* <charflagslen> */
	for (i = 128; i < 256; ++i)
	{
	    flags = 0;
	    if (spelltab.st_isw[i])
		flags |= CF_WORD;
	    if (spelltab.st_isu[i])
		flags |= CF_UPPER;
	    fputc(flags, fd);				/* <charflags> */
	}

	put_bytes(fd, (long_u)l, 2);			/* <folcharslen> */
	fwv &= fwrite(folchars, (size_t)l, (size_t)1, fd); /* <folchars> */
    }

    /* SN_MIDWORD: <midword> */
    if (spin->si_midword != NULL)
    {
	putc(SN_MIDWORD, fd);				/* <sectionID> */
	putc(SNF_REQUIRED, fd);				/* <sectionflags> */

	i = (int)STRLEN(spin->si_midword);
	put_bytes(fd, (long_u)i, 4);			/* <sectionlen> */
	fwv &= fwrite(spin->si_midword, (size_t)i, (size_t)1, fd);
							/* <midword> */
    }

    /* SN_PREFCOND: <prefcondcnt> <prefcond> ... */
    if (spin->si_prefcond.ga_len > 0)
    {
	putc(SN_PREFCOND, fd);				/* <sectionID> */
	putc(SNF_REQUIRED, fd);				/* <sectionflags> */

	l = write_spell_prefcond(NULL, &spin->si_prefcond);
	put_bytes(fd, (long_u)l, 4);			/* <sectionlen> */

	write_spell_prefcond(fd, &spin->si_prefcond);
    }

    /* SN_REP: <repcount> <rep> ...
     * SN_SAL: <salflags> <salcount> <sal> ...
     * SN_REPSAL: <repcount> <rep> ... */

    /* round 1: SN_REP section
     * round 2: SN_SAL section (unless SN_SOFO is used)
     * round 3: SN_REPSAL section */
    for (round = 1; round <= 3; ++round)
    {
	if (round == 1)
	    gap = &spin->si_rep;
	else if (round == 2)
	{
	    /* Don't write SN_SAL when using a SN_SOFO section */
	    if (spin->si_sofofr != NULL && spin->si_sofoto != NULL)
		continue;
	    gap = &spin->si_sal;
	}
	else
	    gap = &spin->si_repsal;

	/* Don't write the section if there are no items. */
	if (gap->ga_len == 0)
	    continue;

	/* Sort the REP/REPSAL items. */
	if (round != 2)
	    qsort(gap->ga_data, (size_t)gap->ga_len,
					       sizeof(fromto_T), rep_compare);

	i = round == 1 ? SN_REP : (round == 2 ? SN_SAL : SN_REPSAL);
	putc(i, fd);					/* <sectionID> */

	/* This is for making suggestions, section is not required. */
	putc(0, fd);					/* <sectionflags> */

	/* Compute the length of what follows. */
	l = 2;	    /* count <repcount> or <salcount> */
	for (i = 0; i < gap->ga_len; ++i)
	{
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    l += 1 + (int)STRLEN(ftp->ft_from);  /* count <*fromlen> and <*from> */
	    l += 1 + (int)STRLEN(ftp->ft_to);    /* count <*tolen> and <*to> */
	}
	if (round == 2)
	    ++l;	/* count <salflags> */
	put_bytes(fd, (long_u)l, 4);			/* <sectionlen> */

	if (round == 2)
	{
	    i = 0;
	    if (spin->si_followup)
		i |= SAL_F0LLOWUP;
	    if (spin->si_collapse)
		i |= SAL_COLLAPSE;
	    if (spin->si_rem_accents)
		i |= SAL_REM_ACCENTS;
	    putc(i, fd);			/* <salflags> */
	}

	put_bytes(fd, (long_u)gap->ga_len, 2);	/* <repcount> or <salcount> */
	for (i = 0; i < gap->ga_len; ++i)
	{
	    /* <rep> : <repfromlen> <repfrom> <reptolen> <repto> */
	    /* <sal> : <salfromlen> <salfrom> <saltolen> <salto> */
	    ftp = &((fromto_T *)gap->ga_data)[i];
	    for (rr = 1; rr <= 2; ++rr)
	    {
		p = rr == 1 ? ftp->ft_from : ftp->ft_to;
		l = (int)STRLEN(p);
		putc(l, fd);
		if (l > 0)
		    fwv &= fwrite(p, l, (size_t)1, fd);
	    }
	}

    }

    /* SN_SOFO: <sofofromlen> <sofofrom> <sofotolen> <sofoto>
     * This is for making suggestions, section is not required. */
    if (spin->si_sofofr != NULL && spin->si_sofoto != NULL)
    {
	putc(SN_SOFO, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */

	l = (int)STRLEN(spin->si_sofofr);
	put_bytes(fd, (long_u)(l + STRLEN(spin->si_sofoto) + 4), 4);
							/* <sectionlen> */

	put_bytes(fd, (long_u)l, 2);			/* <sofofromlen> */
	fwv &= fwrite(spin->si_sofofr, l, (size_t)1, fd); /* <sofofrom> */

	l = (int)STRLEN(spin->si_sofoto);
	put_bytes(fd, (long_u)l, 2);			/* <sofotolen> */
	fwv &= fwrite(spin->si_sofoto, l, (size_t)1, fd); /* <sofoto> */
    }

    /* SN_WORDS: <word> ...
     * This is for making suggestions, section is not required. */
    if (spin->si_commonwords.ht_used > 0)
    {
	putc(SN_WORDS, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */

	/* round 1: count the bytes
	 * round 2: write the bytes */
	for (round = 1; round <= 2; ++round)
	{
	    int		todo;
	    int		len = 0;
	    hashitem_T	*hi;

	    todo = (int)spin->si_commonwords.ht_used;
	    for (hi = spin->si_commonwords.ht_array; todo > 0; ++hi)
		if (!HASHITEM_EMPTY(hi))
		{
		    l = (int)STRLEN(hi->hi_key) + 1;
		    len += l;
		    if (round == 2)			/* <word> */
			fwv &= fwrite(hi->hi_key, (size_t)l, (size_t)1, fd);
		    --todo;
		}
	    if (round == 1)
		put_bytes(fd, (long_u)len, 4);		/* <sectionlen> */
	}
    }

    /* SN_MAP: <mapstr>
     * This is for making suggestions, section is not required. */
    if (spin->si_map.ga_len > 0)
    {
	putc(SN_MAP, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */
	l = spin->si_map.ga_len;
	put_bytes(fd, (long_u)l, 4);			/* <sectionlen> */
	fwv &= fwrite(spin->si_map.ga_data, (size_t)l, (size_t)1, fd);
							/* <mapstr> */
    }

    /* SN_SUGFILE: <timestamp>
     * This is used to notify that a .sug file may be available and at the
     * same time allows for checking that a .sug file that is found matches
     * with this .spl file.  That's because the word numbers must be exactly
     * right. */
    if (!spin->si_nosugfile
	    && (spin->si_sal.ga_len > 0
		     || (spin->si_sofofr != NULL && spin->si_sofoto != NULL)))
    {
	putc(SN_SUGFILE, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */
	put_bytes(fd, (long_u)8, 4);			/* <sectionlen> */

	/* Set si_sugtime and write it to the file. */
	spin->si_sugtime = time(NULL);
	put_time(fd, spin->si_sugtime);			/* <timestamp> */
    }

    /* SN_NOSPLITSUGS: nothing
     * This is used to notify that no suggestions with word splits are to be
     * made. */
    if (spin->si_nosplitsugs)
    {
	putc(SN_NOSPLITSUGS, fd);			/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */
	put_bytes(fd, (long_u)0, 4);			/* <sectionlen> */
    }

    /* SN_NOCOMPUNDSUGS: nothing
     * This is used to notify that no suggestions with compounds are to be
     * made. */
    if (spin->si_nocompoundsugs)
    {
	putc(SN_NOCOMPOUNDSUGS, fd);			/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */
	put_bytes(fd, (long_u)0, 4);			/* <sectionlen> */
    }

    /* SN_COMPOUND: compound info.
     * We don't mark it required, when not supported all compound words will
     * be bad words. */
    if (spin->si_compflags != NULL)
    {
	putc(SN_COMPOUND, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */

	l = (int)STRLEN(spin->si_compflags);
	for (i = 0; i < spin->si_comppat.ga_len; ++i)
	    l += (int)STRLEN(((char_u **)(spin->si_comppat.ga_data))[i]) + 1;
	put_bytes(fd, (long_u)(l + 7), 4);		/* <sectionlen> */

	putc(spin->si_compmax, fd);			/* <compmax> */
	putc(spin->si_compminlen, fd);			/* <compminlen> */
	putc(spin->si_compsylmax, fd);			/* <compsylmax> */
	putc(0, fd);		/* for Vim 7.0b compatibility */
	putc(spin->si_compoptions, fd);			/* <compoptions> */
	put_bytes(fd, (long_u)spin->si_comppat.ga_len, 2);
							/* <comppatcount> */
	for (i = 0; i < spin->si_comppat.ga_len; ++i)
	{
	    p = ((char_u **)(spin->si_comppat.ga_data))[i];
	    putc((int)STRLEN(p), fd);			/* <comppatlen> */
	    fwv &= fwrite(p, (size_t)STRLEN(p), (size_t)1, fd);
							/* <comppattext> */
	}
							/* <compflags> */
	fwv &= fwrite(spin->si_compflags, (size_t)STRLEN(spin->si_compflags),
							       (size_t)1, fd);
    }

    /* SN_NOBREAK: NOBREAK flag */
    if (spin->si_nobreak)
    {
	putc(SN_NOBREAK, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */

	/* It's empty, the presence of the section flags the feature. */
	put_bytes(fd, (long_u)0, 4);			/* <sectionlen> */
    }

    /* SN_SYLLABLE: syllable info.
     * We don't mark it required, when not supported syllables will not be
     * counted. */
    if (spin->si_syllable != NULL)
    {
	putc(SN_SYLLABLE, fd);				/* <sectionID> */
	putc(0, fd);					/* <sectionflags> */

	l = (int)STRLEN(spin->si_syllable);
	put_bytes(fd, (long_u)l, 4);			/* <sectionlen> */
	fwv &= fwrite(spin->si_syllable, (size_t)l, (size_t)1, fd);
							/* <syllable> */
    }

    /* end of <SECTIONS> */
    putc(SN_END, fd);					/* <sectionend> */


    /*
     * <LWORDTREE>  <KWORDTREE>  <PREFIXTREE>
     */
    spin->si_memtot = 0;
    for (round = 1; round <= 3; ++round)
    {
	if (round == 1)
	    tree = spin->si_foldroot->wn_sibling;
	else if (round == 2)
	    tree = spin->si_keeproot->wn_sibling;
	else
	    tree = spin->si_prefroot->wn_sibling;

	/* Clear the index and wnode fields in the tree. */
	clear_node(tree);

	/* Count the number of nodes.  Needed to be able to allocate the
	 * memory when reading the nodes.  Also fills in index for shared
	 * nodes. */
	nodecount = put_node(NULL, tree, 0, regionmask, round == 3);

	/* number of nodes in 4 bytes */
	put_bytes(fd, (long_u)nodecount, 4);	/* <nodecount> */
	spin->si_memtot += nodecount + nodecount * sizeof(int);

	/* Write the nodes. */
	(void)put_node(fd, tree, 0, regionmask, round == 3);
    }

    /* Write another byte to check for errors (file system full). */
    if (putc(0, fd) == EOF)
	retval = FAIL;
theend:
    if (fclose(fd) == EOF)
	retval = FAIL;

    if (fwv != (size_t)1)
	retval = FAIL;
    if (retval == FAIL)
	EMSG(_(e_write));

    return retval;
}