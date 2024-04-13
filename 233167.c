spell_read_dic(spellinfo_T *spin, char_u *fname, afffile_T *affile)
{
    hashtab_T	ht;
    char_u	line[MAXLINELEN];
    char_u	*p;
    char_u	*afflist;
    char_u	store_afflist[MAXWLEN];
    int		pfxlen;
    int		need_affix;
    char_u	*dw;
    char_u	*pc;
    char_u	*w;
    int		l;
    hash_T	hash;
    hashitem_T	*hi;
    FILE	*fd;
    int		lnum = 1;
    int		non_ascii = 0;
    int		retval = OK;
    char_u	message[MAXLINELEN + MAXWLEN];
    int		flags;
    int		duplicate = 0;

    /*
     * Open the file.
     */
    fd = mch_fopen((char *)fname, "r");
    if (fd == NULL)
    {
	EMSG2(_(e_notopen), fname);
	return FAIL;
    }

    /* The hashtable is only used to detect duplicated words. */
    hash_init(&ht);

    vim_snprintf((char *)IObuff, IOSIZE,
				  _("Reading dictionary file %s ..."), fname);
    spell_message(spin, IObuff);

    /* start with a message for the first line */
    spin->si_msg_count = 999999;

    /* Read and ignore the first line: word count. */
    (void)vim_fgets(line, MAXLINELEN, fd);
    if (!vim_isdigit(*skipwhite(line)))
	EMSG2(_("E760: No word count in %s"), fname);

    /*
     * Read all the lines in the file one by one.
     * The words are converted to 'encoding' here, before being added to
     * the hashtable.
     */
    while (!vim_fgets(line, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;
	if (line[0] == '#' || line[0] == '/')
	    continue;	/* comment line */

	/* Remove CR, LF and white space from the end.  White space halfway
	 * the word is kept to allow e.g., "et al.". */
	l = (int)STRLEN(line);
	while (l > 0 && line[l - 1] <= ' ')
	    --l;
	if (l == 0)
	    continue;	/* empty line */
	line[l] = NUL;

#ifdef FEAT_MBYTE
	/* Convert from "SET" to 'encoding' when needed. */
	if (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, line, NULL);
	    if (pc == NULL)
	    {
		smsg((char_u *)_("Conversion failure for word in %s line %d: %s"),
						       fname, lnum, line);
		continue;
	    }
	    w = pc;
	}
	else
#endif
	{
	    pc = NULL;
	    w = line;
	}

	/* Truncate the word at the "/", set "afflist" to what follows.
	 * Replace "\/" by "/" and "\\" by "\". */
	afflist = NULL;
	for (p = w; *p != NUL; mb_ptr_adv(p))
	{
	    if (*p == '\\' && (p[1] == '\\' || p[1] == '/'))
		STRMOVE(p, p + 1);
	    else if (*p == '/')
	    {
		*p = NUL;
		afflist = p + 1;
		break;
	    }
	}

	/* Skip non-ASCII words when "spin->si_ascii" is TRUE. */
	if (spin->si_ascii && has_non_ascii(w))
	{
	    ++non_ascii;
	    vim_free(pc);
	    continue;
	}

	/* This takes time, print a message every 10000 words. */
	if (spin->si_verbose && spin->si_msg_count > 10000)
	{
	    spin->si_msg_count = 0;
	    vim_snprintf((char *)message, sizeof(message),
		    _("line %6d, word %6d - %s"),
		       lnum, spin->si_foldwcount + spin->si_keepwcount, w);
	    msg_start();
	    msg_puts_long_attr(message, 0);
	    msg_clr_eos();
	    msg_didout = FALSE;
	    msg_col = 0;
	    out_flush();
	}

	/* Store the word in the hashtable to be able to find duplicates. */
	dw = (char_u *)getroom_save(spin, w);
	if (dw == NULL)
	{
	    retval = FAIL;
	    vim_free(pc);
	    break;
	}

	hash = hash_hash(dw);
	hi = hash_lookup(&ht, dw, hash);
	if (!HASHITEM_EMPTY(hi))
	{
	    if (p_verbose > 0)
		smsg((char_u *)_("Duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    else if (duplicate == 0)
		smsg((char_u *)_("First duplicate word in %s line %d: %s"),
							     fname, lnum, dw);
	    ++duplicate;
	}
	else
	    hash_add_item(&ht, hi, dw, hash);

	flags = 0;
	store_afflist[0] = NUL;
	pfxlen = 0;
	need_affix = FALSE;
	if (afflist != NULL)
	{
	    /* Extract flags from the affix list. */
	    flags |= get_affix_flags(affile, afflist);

	    if (affile->af_needaffix != 0 && flag_in_afflist(
			  affile->af_flagtype, afflist, affile->af_needaffix))
		need_affix = TRUE;

	    if (affile->af_pfxpostpone)
		/* Need to store the list of prefix IDs with the word. */
		pfxlen = get_pfxlist(affile, afflist, store_afflist);

	    if (spin->si_compflags != NULL)
		/* Need to store the list of compound flags with the word.
		 * Concatenate them to the list of prefix IDs. */
		get_compflags(affile, afflist, store_afflist + pfxlen);
	}

	/* Add the word to the word tree(s). */
	if (store_word(spin, dw, flags, spin->si_region,
					   store_afflist, need_affix) == FAIL)
	    retval = FAIL;

	if (afflist != NULL)
	{
	    /* Find all matching suffixes and add the resulting words.
	     * Additionally do matching prefixes that combine. */
	    if (store_aff_word(spin, dw, afflist, affile,
			   &affile->af_suff, &affile->af_pref,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;

	    /* Find all matching prefixes and add the resulting words. */
	    if (store_aff_word(spin, dw, afflist, affile,
			  &affile->af_pref, NULL,
			    CONDIT_SUF, flags, store_afflist, pfxlen) == FAIL)
		retval = FAIL;
	}

	vim_free(pc);
    }

    if (duplicate > 0)
	smsg((char_u *)_("%d duplicate word(s) in %s"), duplicate, fname);
    if (spin->si_ascii && non_ascii > 0)
	smsg((char_u *)_("Ignored %d word(s) with non-ASCII characters in %s"),
							    non_ascii, fname);
    hash_clear(&ht);

    fclose(fd);
    return retval;
}