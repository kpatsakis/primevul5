mkspell(
    int		fcount,
    char_u	**fnames,
    int		ascii,		    /* -ascii argument given */
    int		over_write,	    /* overwrite existing output file */
    int		added_word)	    /* invoked through "zg" */
{
    char_u	*fname = NULL;
    char_u	*wfname;
    char_u	**innames;
    int		incount;
    afffile_T	*(afile[8]);
    int		i;
    int		len;
    stat_T	st;
    int		error = FALSE;
    spellinfo_T spin;

    vim_memset(&spin, 0, sizeof(spin));
    spin.si_verbose = !added_word;
    spin.si_ascii = ascii;
    spin.si_followup = TRUE;
    spin.si_rem_accents = TRUE;
    ga_init2(&spin.si_rep, (int)sizeof(fromto_T), 20);
    ga_init2(&spin.si_repsal, (int)sizeof(fromto_T), 20);
    ga_init2(&spin.si_sal, (int)sizeof(fromto_T), 20);
    ga_init2(&spin.si_map, (int)sizeof(char_u), 100);
    ga_init2(&spin.si_comppat, (int)sizeof(char_u *), 20);
    ga_init2(&spin.si_prefcond, (int)sizeof(char_u *), 50);
    hash_init(&spin.si_commonwords);
    spin.si_newcompID = 127;	/* start compound ID at first maximum */

    /* default: fnames[0] is output file, following are input files */
    innames = &fnames[1];
    incount = fcount - 1;

    wfname = alloc(MAXPATHL);
    if (wfname == NULL)
	return;

    if (fcount >= 1)
    {
	len = (int)STRLEN(fnames[0]);
	if (fcount == 1 && len > 4 && STRCMP(fnames[0] + len - 4, ".add") == 0)
	{
	    /* For ":mkspell path/en.latin1.add" output file is
	     * "path/en.latin1.add.spl". */
	    innames = &fnames[0];
	    incount = 1;
	    vim_snprintf((char *)wfname, MAXPATHL, "%s.spl", fnames[0]);
	}
	else if (fcount == 1)
	{
	    /* For ":mkspell path/vim" output file is "path/vim.latin1.spl". */
	    innames = &fnames[0];
	    incount = 1;
	    vim_snprintf((char *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());
	}
	else if (len > 4 && STRCMP(fnames[0] + len - 4, ".spl") == 0)
	{
	    /* Name ends in ".spl", use as the file name. */
	    vim_strncpy(wfname, fnames[0], MAXPATHL - 1);
	}
	else
	    /* Name should be language, make the file name from it. */
	    vim_snprintf((char *)wfname, MAXPATHL, SPL_FNAME_TMPL,
		  fnames[0], spin.si_ascii ? (char_u *)"ascii" : spell_enc());

	/* Check for .ascii.spl. */
	if (strstr((char *)gettail(wfname), SPL_FNAME_ASCII) != NULL)
	    spin.si_ascii = TRUE;

	/* Check for .add.spl. */
	if (strstr((char *)gettail(wfname), SPL_FNAME_ADD) != NULL)
	    spin.si_add = TRUE;
    }

    if (incount <= 0)
	EMSG(_(e_invarg));	/* need at least output and input names */
    else if (vim_strchr(gettail(wfname), '_') != NULL)
	EMSG(_("E751: Output file name must not have region name"));
    else if (incount > 8)
	EMSG(_("E754: Only up to 8 regions supported"));
    else
    {
	/* Check for overwriting before doing things that may take a lot of
	 * time. */
	if (!over_write && mch_stat((char *)wfname, &st) >= 0)
	{
	    EMSG(_(e_exists));
	    goto theend;
	}
	if (mch_isdir(wfname))
	{
	    EMSG2(_(e_isadir2), wfname);
	    goto theend;
	}

	fname = alloc(MAXPATHL);
	if (fname == NULL)
	    goto theend;

	/*
	 * Init the aff and dic pointers.
	 * Get the region names if there are more than 2 arguments.
	 */
	for (i = 0; i < incount; ++i)
	{
	    afile[i] = NULL;

	    if (incount > 1)
	    {
		len = (int)STRLEN(innames[i]);
		if (STRLEN(gettail(innames[i])) < 5
						|| innames[i][len - 3] != '_')
		{
		    EMSG2(_("E755: Invalid region in %s"), innames[i]);
		    goto theend;
		}
		spin.si_region_name[i * 2] = TOLOWER_ASC(innames[i][len - 2]);
		spin.si_region_name[i * 2 + 1] =
					     TOLOWER_ASC(innames[i][len - 1]);
	    }
	}
	spin.si_region_count = incount;

	spin.si_foldroot = wordtree_alloc(&spin);
	spin.si_keeproot = wordtree_alloc(&spin);
	spin.si_prefroot = wordtree_alloc(&spin);
	if (spin.si_foldroot == NULL
		|| spin.si_keeproot == NULL
		|| spin.si_prefroot == NULL)
	{
	    free_blocks(spin.si_blocks);
	    goto theend;
	}

	/* When not producing a .add.spl file clear the character table when
	 * we encounter one in the .aff file.  This means we dump the current
	 * one in the .spl file if the .aff file doesn't define one.  That's
	 * better than guessing the contents, the table will match a
	 * previously loaded spell file. */
	if (!spin.si_add)
	    spin.si_clear_chartab = TRUE;

	/*
	 * Read all the .aff and .dic files.
	 * Text is converted to 'encoding'.
	 * Words are stored in the case-folded and keep-case trees.
	 */
	for (i = 0; i < incount && !error; ++i)
	{
	    spin.si_conv.vc_type = CONV_NONE;
	    spin.si_region = 1 << i;

	    vim_snprintf((char *)fname, MAXPATHL, "%s.aff", innames[i]);
	    if (mch_stat((char *)fname, &st) >= 0)
	    {
		/* Read the .aff file.  Will init "spin->si_conv" based on the
		 * "SET" line. */
		afile[i] = spell_read_aff(&spin, fname);
		if (afile[i] == NULL)
		    error = TRUE;
		else
		{
		    /* Read the .dic file and store the words in the trees. */
		    vim_snprintf((char *)fname, MAXPATHL, "%s.dic",
								  innames[i]);
		    if (spell_read_dic(&spin, fname, afile[i]) == FAIL)
			error = TRUE;
		}
	    }
	    else
	    {
		/* No .aff file, try reading the file as a word list.  Store
		 * the words in the trees. */
		if (spell_read_wordfile(&spin, innames[i]) == FAIL)
		    error = TRUE;
	    }

#ifdef FEAT_MBYTE
	    /* Free any conversion stuff. */
	    convert_setup(&spin.si_conv, NULL, NULL);
#endif
	}

	if (spin.si_compflags != NULL && spin.si_nobreak)
	    MSG(_("Warning: both compounding and NOBREAK specified"));

	if (!error && !got_int)
	{
	    /*
	     * Combine tails in the tree.
	     */
	    spell_message(&spin, (char_u *)_(msg_compressing));
	    wordtree_compress(&spin, spin.si_foldroot);
	    wordtree_compress(&spin, spin.si_keeproot);
	    wordtree_compress(&spin, spin.si_prefroot);
	}

	if (!error && !got_int)
	{
	    /*
	     * Write the info in the spell file.
	     */
	    vim_snprintf((char *)IObuff, IOSIZE,
				      _("Writing spell file %s ..."), wfname);
	    spell_message(&spin, IObuff);

	    error = write_vim_spell(&spin, wfname) == FAIL;

	    spell_message(&spin, (char_u *)_("Done!"));
	    vim_snprintf((char *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin.si_memtot);
	    spell_message(&spin, IObuff);

	    /*
	     * If the file is loaded need to reload it.
	     */
	    if (!error)
		spell_reload_one(wfname, added_word);
	}

	/* Free the allocated memory. */
	ga_clear(&spin.si_rep);
	ga_clear(&spin.si_repsal);
	ga_clear(&spin.si_sal);
	ga_clear(&spin.si_map);
	ga_clear(&spin.si_comppat);
	ga_clear(&spin.si_prefcond);
	hash_clear_all(&spin.si_commonwords, 0);

	/* Free the .aff file structures. */
	for (i = 0; i < incount; ++i)
	    if (afile[i] != NULL)
		spell_free_aff(afile[i]);

	/* Free all the bits and pieces at once. */
	free_blocks(spin.si_blocks);

	/*
	 * If there is soundfolding info and no NOSUGFILE item create the
	 * .sug file with the soundfolded word trie.
	 */
	if (spin.si_sugtime != 0 && !error && !got_int)
	    spell_make_sugfile(&spin, wfname);

    }

theend:
    vim_free(fname);
    vim_free(wfname);
}