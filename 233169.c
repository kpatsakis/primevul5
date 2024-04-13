spell_read_aff(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    afffile_T	*aff;
    char_u	rline[MAXLINELEN];
    char_u	*line;
    char_u	*pc = NULL;
#define MAXITEMCNT  30
    char_u	*(items[MAXITEMCNT]);
    int		itemcnt;
    char_u	*p;
    int		lnum = 0;
    affheader_T	*cur_aff = NULL;
    int		did_postpone_prefix = FALSE;
    int		aff_todo = 0;
    hashtab_T	*tp;
    char_u	*low = NULL;
    char_u	*fol = NULL;
    char_u	*upp = NULL;
    int		do_rep;
    int		do_repsal;
    int		do_sal;
    int		do_mapline;
    int		found_map = FALSE;
    hashitem_T	*hi;
    int		l;
    int		compminlen = 0;		/* COMPOUNDMIN value */
    int		compsylmax = 0;		/* COMPOUNDSYLMAX value */
    int		compoptions = 0;	/* COMP_ flags */
    int		compmax = 0;		/* COMPOUNDWORDMAX value */
    char_u	*compflags = NULL;	/* COMPOUNDFLAG and COMPOUNDRULE
					   concatenated */
    char_u	*midword = NULL;	/* MIDWORD value */
    char_u	*syllable = NULL;	/* SYLLABLE value */
    char_u	*sofofrom = NULL;	/* SOFOFROM value */
    char_u	*sofoto = NULL;		/* SOFOTO value */

    /*
     * Open the file.
     */
    fd = mch_fopen((char *)fname, "r");
    if (fd == NULL)
    {
	EMSG2(_(e_notopen), fname);
	return NULL;
    }

    vim_snprintf((char *)IObuff, IOSIZE, _("Reading affix file %s ..."), fname);
    spell_message(spin, IObuff);

    /* Only do REP lines when not done in another .aff file already. */
    do_rep = spin->si_rep.ga_len == 0;

    /* Only do REPSAL lines when not done in another .aff file already. */
    do_repsal = spin->si_repsal.ga_len == 0;

    /* Only do SAL lines when not done in another .aff file already. */
    do_sal = spin->si_sal.ga_len == 0;

    /* Only do MAP lines when not done in another .aff file already. */
    do_mapline = spin->si_map.ga_len == 0;

    /*
     * Allocate and init the afffile_T structure.
     */
    aff = (afffile_T *)getroom(spin, sizeof(afffile_T), TRUE);
    if (aff == NULL)
    {
	fclose(fd);
	return NULL;
    }
    hash_init(&aff->af_pref);
    hash_init(&aff->af_suff);
    hash_init(&aff->af_comp);

    /*
     * Read all the lines in the file one by one.
     */
    while (!vim_fgets(rline, MAXLINELEN, fd) && !got_int)
    {
	line_breakcheck();
	++lnum;

	/* Skip comment lines. */
	if (*rline == '#')
	    continue;

	/* Convert from "SET" to 'encoding' when needed. */
	vim_free(pc);
#ifdef FEAT_MBYTE
	if (spin->si_conv.vc_type != CONV_NONE)
	{
	    pc = string_convert(&spin->si_conv, rline, NULL);
	    if (pc == NULL)
	    {
		smsg((char_u *)_("Conversion failure for word in %s line %d: %s"),
							   fname, lnum, rline);
		continue;
	    }
	    line = pc;
	}
	else
#endif
	{
	    pc = NULL;
	    line = rline;
	}

	/* Split the line up in white separated items.  Put a NUL after each
	 * item. */
	itemcnt = 0;
	for (p = line; ; )
	{
	    while (*p != NUL && *p <= ' ')  /* skip white space and CR/NL */
		++p;
	    if (*p == NUL)
		break;
	    if (itemcnt == MAXITEMCNT)	    /* too many items */
		break;
	    items[itemcnt++] = p;
	    /* A few items have arbitrary text argument, don't split them. */
	    if (itemcnt == 2 && spell_info_item(items[0]))
		while (*p >= ' ' || *p == TAB)    /* skip until CR/NL */
		    ++p;
	    else
		while (*p > ' ')    /* skip until white space or CR/NL */
		    ++p;
	    if (*p == NUL)
		break;
	    *p++ = NUL;
	}

	/* Handle non-empty lines. */
	if (itemcnt > 0)
	{
	    if (is_aff_rule(items, itemcnt, "SET", 2) && aff->af_enc == NULL)
	    {
#ifdef FEAT_MBYTE
		/* Setup for conversion from "ENC" to 'encoding'. */
		aff->af_enc = enc_canonize(items[1]);
		if (aff->af_enc != NULL && !spin->si_ascii
			&& convert_setup(&spin->si_conv, aff->af_enc,
							       p_enc) == FAIL)
		    smsg((char_u *)_("Conversion in %s not supported: from %s to %s"),
					       fname, aff->af_enc, p_enc);
		spin->si_conv.vc_fail = TRUE;
#else
		    smsg((char_u *)_("Conversion in %s not supported"), fname);
#endif
	    }
	    else if (is_aff_rule(items, itemcnt, "FLAG", 2)
					      && aff->af_flagtype == AFT_CHAR)
	    {
		if (STRCMP(items[1], "long") == 0)
		    aff->af_flagtype = AFT_LONG;
		else if (STRCMP(items[1], "num") == 0)
		    aff->af_flagtype = AFT_NUM;
		else if (STRCMP(items[1], "caplong") == 0)
		    aff->af_flagtype = AFT_CAPLONG;
		else
		    smsg((char_u *)_("Invalid value for FLAG in %s line %d: %s"),
			    fname, lnum, items[1]);
		if (aff->af_rare != 0
			|| aff->af_keepcase != 0
			|| aff->af_bad != 0
			|| aff->af_needaffix != 0
			|| aff->af_circumfix != 0
			|| aff->af_needcomp != 0
			|| aff->af_comproot != 0
			|| aff->af_nosuggest != 0
			|| compflags != NULL
			|| aff->af_suff.ht_used > 0
			|| aff->af_pref.ht_used > 0)
		    smsg((char_u *)_("FLAG after using flags in %s line %d: %s"),
			    fname, lnum, items[1]);
	    }
	    else if (spell_info_item(items[0]))
	    {
		    p = (char_u *)getroom(spin,
			    (spin->si_info == NULL ? 0 : STRLEN(spin->si_info))
			    + STRLEN(items[0])
			    + STRLEN(items[1]) + 3, FALSE);
		    if (p != NULL)
		    {
			if (spin->si_info != NULL)
			{
			    STRCPY(p, spin->si_info);
			    STRCAT(p, "\n");
			}
			STRCAT(p, items[0]);
			STRCAT(p, " ");
			STRCAT(p, items[1]);
			spin->si_info = p;
		    }
	    }
	    else if (is_aff_rule(items, itemcnt, "MIDWORD", 2)
							   && midword == NULL)
	    {
		midword = getroom_save(spin, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "TRY", 2))
	    {
		/* ignored, we look in the tree for what chars may appear */
	    }
	    /* TODO: remove "RAR" later */
	    else if ((is_aff_rule(items, itemcnt, "RAR", 2)
			|| is_aff_rule(items, itemcnt, "RARE", 2))
							 && aff->af_rare == 0)
	    {
		aff->af_rare = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    /* TODO: remove "KEP" later */
	    else if ((is_aff_rule(items, itemcnt, "KEP", 2)
			|| is_aff_rule(items, itemcnt, "KEEPCASE", 2))
						     && aff->af_keepcase == 0)
	    {
		aff->af_keepcase = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    else if ((is_aff_rule(items, itemcnt, "BAD", 2)
			|| is_aff_rule(items, itemcnt, "FORBIDDENWORD", 2))
							  && aff->af_bad == 0)
	    {
		aff->af_bad = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    else if (is_aff_rule(items, itemcnt, "NEEDAFFIX", 2)
						    && aff->af_needaffix == 0)
	    {
		aff->af_needaffix = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    else if (is_aff_rule(items, itemcnt, "CIRCUMFIX", 2)
						    && aff->af_circumfix == 0)
	    {
		aff->af_circumfix = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    else if (is_aff_rule(items, itemcnt, "NOSUGGEST", 2)
						    && aff->af_nosuggest == 0)
	    {
		aff->af_nosuggest = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    else if ((is_aff_rule(items, itemcnt, "NEEDCOMPOUND", 2)
			|| is_aff_rule(items, itemcnt, "ONLYINCOMPOUND", 2))
						     && aff->af_needcomp == 0)
	    {
		aff->af_needcomp = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDROOT", 2)
						     && aff->af_comproot == 0)
	    {
		aff->af_comproot = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDFORBIDFLAG", 2)
						   && aff->af_compforbid == 0)
	    {
		aff->af_compforbid = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		if (aff->af_pref.ht_used > 0)
		    smsg((char_u *)_("Defining COMPOUNDFORBIDFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDPERMITFLAG", 2)
						   && aff->af_comppermit == 0)
	    {
		aff->af_comppermit = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		if (aff->af_pref.ht_used > 0)
		    smsg((char_u *)_("Defining COMPOUNDPERMITFLAG after PFX item may give wrong results in %s line %d"),
			    fname, lnum);
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDFLAG", 2)
							 && compflags == NULL)
	    {
		/* Turn flag "c" into COMPOUNDRULE compatible string "c+",
		 * "Na" into "Na+", "1234" into "1234+". */
		p = getroom(spin, STRLEN(items[1]) + 2, FALSE);
		if (p != NULL)
		{
		    STRCPY(p, items[1]);
		    STRCAT(p, "+");
		    compflags = p;
		}
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDRULES", 2))
	    {
		/* We don't use the count, but do check that it's a number and
		 * not COMPOUNDRULE mistyped. */
		if (atoi((char *)items[1]) == 0)
		    smsg((char_u *)_("Wrong COMPOUNDRULES value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDRULE", 2))
	    {
		/* Don't use the first rule if it is a number. */
		if (compflags != NULL || *skipdigits(items[1]) != NUL)
		{
		    /* Concatenate this string to previously defined ones,
		     * using a slash to separate them. */
		    l = (int)STRLEN(items[1]) + 1;
		    if (compflags != NULL)
			l += (int)STRLEN(compflags) + 1;
		    p = getroom(spin, l, FALSE);
		    if (p != NULL)
		    {
			if (compflags != NULL)
			{
			    STRCPY(p, compflags);
			    STRCAT(p, "/");
			}
			STRCAT(p, items[1]);
			compflags = p;
		    }
		}
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDWORDMAX", 2)
							      && compmax == 0)
	    {
		compmax = atoi((char *)items[1]);
		if (compmax == 0)
		    smsg((char_u *)_("Wrong COMPOUNDWORDMAX value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDMIN", 2)
							   && compminlen == 0)
	    {
		compminlen = atoi((char *)items[1]);
		if (compminlen == 0)
		    smsg((char_u *)_("Wrong COMPOUNDMIN value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "COMPOUNDSYLMAX", 2)
							   && compsylmax == 0)
	    {
		compsylmax = atoi((char *)items[1]);
		if (compsylmax == 0)
		    smsg((char_u *)_("Wrong COMPOUNDSYLMAX value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDDUP", 1))
	    {
		compoptions |= COMP_CHECKDUP;
	    }
	    else if (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDREP", 1))
	    {
		compoptions |= COMP_CHECKREP;
	    }
	    else if (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDCASE", 1))
	    {
		compoptions |= COMP_CHECKCASE;
	    }
	    else if (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDTRIPLE", 1))
	    {
		compoptions |= COMP_CHECKTRIPLE;
	    }
	    else if (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDPATTERN", 2))
	    {
		if (atoi((char *)items[1]) == 0)
		    smsg((char_u *)_("Wrong CHECKCOMPOUNDPATTERN value in %s line %d: %s"),
						       fname, lnum, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "CHECKCOMPOUNDPATTERN", 3))
	    {
		garray_T    *gap = &spin->si_comppat;
		int	    i;

		/* Only add the couple if it isn't already there. */
		for (i = 0; i < gap->ga_len - 1; i += 2)
		    if (STRCMP(((char_u **)(gap->ga_data))[i], items[1]) == 0
			    && STRCMP(((char_u **)(gap->ga_data))[i + 1],
							       items[2]) == 0)
			break;
		if (i >= gap->ga_len && ga_grow(gap, 2) == OK)
		{
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, items[1]);
		    ((char_u **)(gap->ga_data))[gap->ga_len++]
					       = getroom_save(spin, items[2]);
		}
	    }
	    else if (is_aff_rule(items, itemcnt, "SYLLABLE", 2)
							  && syllable == NULL)
	    {
		syllable = getroom_save(spin, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "NOBREAK", 1))
	    {
		spin->si_nobreak = TRUE;
	    }
	    else if (is_aff_rule(items, itemcnt, "NOSPLITSUGS", 1))
	    {
		spin->si_nosplitsugs = TRUE;
	    }
	    else if (is_aff_rule(items, itemcnt, "NOCOMPOUNDSUGS", 1))
	    {
		spin->si_nocompoundsugs = TRUE;
	    }
	    else if (is_aff_rule(items, itemcnt, "NOSUGFILE", 1))
	    {
		spin->si_nosugfile = TRUE;
	    }
	    else if (is_aff_rule(items, itemcnt, "PFXPOSTPONE", 1))
	    {
		aff->af_pfxpostpone = TRUE;
	    }
	    else if (is_aff_rule(items, itemcnt, "IGNOREEXTRA", 1))
	    {
		aff->af_ignoreextra = TRUE;
	    }
	    else if ((STRCMP(items[0], "PFX") == 0
					      || STRCMP(items[0], "SFX") == 0)
		    && aff_todo == 0
		    && itemcnt >= 4)
	    {
		int	lasti = 4;
		char_u	key[AH_KEY_LEN];

		if (*items[0] == 'P')
		    tp = &aff->af_pref;
		else
		    tp = &aff->af_suff;

		/* Myspell allows the same affix name to be used multiple
		 * times.  The affix files that do this have an undocumented
		 * "S" flag on all but the last block, thus we check for that
		 * and store it in ah_follows. */
		vim_strncpy(key, items[1], AH_KEY_LEN - 1);
		hi = hash_find(tp, key);
		if (!HASHITEM_EMPTY(hi))
		{
		    cur_aff = HI2AH(hi);
		    if (cur_aff->ah_combine != (*items[2] == 'Y'))
			smsg((char_u *)_("Different combining flag in continued affix block in %s line %d: %s"),
						   fname, lnum, items[1]);
		    if (!cur_aff->ah_follows)
			smsg((char_u *)_("Duplicate affix in %s line %d: %s"),
						       fname, lnum, items[1]);
		}
		else
		{
		    /* New affix letter. */
		    cur_aff = (affheader_T *)getroom(spin,
						   sizeof(affheader_T), TRUE);
		    if (cur_aff == NULL)
			break;
		    cur_aff->ah_flag = affitem2flag(aff->af_flagtype, items[1],
								 fname, lnum);
		    if (cur_aff->ah_flag == 0 || STRLEN(items[1]) >= AH_KEY_LEN)
			break;
		    if (cur_aff->ah_flag == aff->af_bad
			    || cur_aff->ah_flag == aff->af_rare
			    || cur_aff->ah_flag == aff->af_keepcase
			    || cur_aff->ah_flag == aff->af_needaffix
			    || cur_aff->ah_flag == aff->af_circumfix
			    || cur_aff->ah_flag == aff->af_nosuggest
			    || cur_aff->ah_flag == aff->af_needcomp
			    || cur_aff->ah_flag == aff->af_comproot)
			smsg((char_u *)_("Affix also used for BAD/RARE/KEEPCASE/NEEDAFFIX/NEEDCOMPOUND/NOSUGGEST in %s line %d: %s"),
						       fname, lnum, items[1]);
		    STRCPY(cur_aff->ah_key, items[1]);
		    hash_add(tp, cur_aff->ah_key);

		    cur_aff->ah_combine = (*items[2] == 'Y');
		}

		/* Check for the "S" flag, which apparently means that another
		 * block with the same affix name is following. */
		if (itemcnt > lasti && STRCMP(items[lasti], "S") == 0)
		{
		    ++lasti;
		    cur_aff->ah_follows = TRUE;
		}
		else
		    cur_aff->ah_follows = FALSE;

		/* Myspell allows extra text after the item, but that might
		 * mean mistakes go unnoticed.  Require a comment-starter. */
		if (itemcnt > lasti && *items[lasti] != '#')
		    smsg((char_u *)_(e_afftrailing), fname, lnum, items[lasti]);

		if (STRCMP(items[2], "Y") != 0 && STRCMP(items[2], "N") != 0)
		    smsg((char_u *)_("Expected Y or N in %s line %d: %s"),
						       fname, lnum, items[2]);

		if (*items[0] == 'P' && aff->af_pfxpostpone)
		{
		    if (cur_aff->ah_newID == 0)
		    {
			/* Use a new number in the .spl file later, to be able
			 * to handle multiple .aff files. */
			check_renumber(spin);
			cur_aff->ah_newID = ++spin->si_newprefID;

			/* We only really use ah_newID if the prefix is
			 * postponed.  We know that only after handling all
			 * the items. */
			did_postpone_prefix = FALSE;
		    }
		    else
			/* Did use the ID in a previous block. */
			did_postpone_prefix = TRUE;
		}

		aff_todo = atoi((char *)items[3]);
	    }
	    else if ((STRCMP(items[0], "PFX") == 0
					      || STRCMP(items[0], "SFX") == 0)
		    && aff_todo > 0
		    && STRCMP(cur_aff->ah_key, items[1]) == 0
		    && itemcnt >= 5)
	    {
		affentry_T	*aff_entry;
		int		upper = FALSE;
		int		lasti = 5;

		/* Myspell allows extra text after the item, but that might
		 * mean mistakes go unnoticed.  Require a comment-starter,
		 * unless IGNOREEXTRA is used.  Hunspell uses a "-" item. */
		if (itemcnt > lasti
			&& !aff->af_ignoreextra
			&& *items[lasti] != '#'
			&& (STRCMP(items[lasti], "-") != 0
						     || itemcnt != lasti + 1))
		    smsg((char_u *)_(e_afftrailing), fname, lnum, items[lasti]);

		/* New item for an affix letter. */
		--aff_todo;
		aff_entry = (affentry_T *)getroom(spin,
						    sizeof(affentry_T), TRUE);
		if (aff_entry == NULL)
		    break;

		if (STRCMP(items[2], "0") != 0)
		    aff_entry->ae_chop = getroom_save(spin, items[2]);
		if (STRCMP(items[3], "0") != 0)
		{
		    aff_entry->ae_add = getroom_save(spin, items[3]);

		    /* Recognize flags on the affix: abcd/XYZ */
		    aff_entry->ae_flags = vim_strchr(aff_entry->ae_add, '/');
		    if (aff_entry->ae_flags != NULL)
		    {
			*aff_entry->ae_flags++ = NUL;
			aff_process_flags(aff, aff_entry);
		    }
		}

		/* Don't use an affix entry with non-ASCII characters when
		 * "spin->si_ascii" is TRUE. */
		if (!spin->si_ascii || !(has_non_ascii(aff_entry->ae_chop)
					  || has_non_ascii(aff_entry->ae_add)))
		{
		    aff_entry->ae_next = cur_aff->ah_first;
		    cur_aff->ah_first = aff_entry;

		    if (STRCMP(items[4], ".") != 0)
		    {
			char_u	buf[MAXLINELEN];

			aff_entry->ae_cond = getroom_save(spin, items[4]);
			if (*items[0] == 'P')
			    sprintf((char *)buf, "^%s", items[4]);
			else
			    sprintf((char *)buf, "%s$", items[4]);
			aff_entry->ae_prog = vim_regcomp(buf,
					    RE_MAGIC + RE_STRING + RE_STRICT);
			if (aff_entry->ae_prog == NULL)
			    smsg((char_u *)_("Broken condition in %s line %d: %s"),
						       fname, lnum, items[4]);
		    }

		    /* For postponed prefixes we need an entry in si_prefcond
		     * for the condition.  Use an existing one if possible.
		     * Can't be done for an affix with flags, ignoring
		     * COMPOUNDFORBIDFLAG and COMPOUNDPERMITFLAG. */
		    if (*items[0] == 'P' && aff->af_pfxpostpone
					       && aff_entry->ae_flags == NULL)
		    {
			/* When the chop string is one lower-case letter and
			 * the add string ends in the upper-case letter we set
			 * the "upper" flag, clear "ae_chop" and remove the
			 * letters from "ae_add".  The condition must either
			 * be empty or start with the same letter. */
			if (aff_entry->ae_chop != NULL
				&& aff_entry->ae_add != NULL
#ifdef FEAT_MBYTE
				&& aff_entry->ae_chop[(*mb_ptr2len)(
						   aff_entry->ae_chop)] == NUL
#else
				&& aff_entry->ae_chop[1] == NUL
#endif
				)
			{
			    int		c, c_up;

			    c = PTR2CHAR(aff_entry->ae_chop);
			    c_up = SPELL_TOUPPER(c);
			    if (c_up != c
				    && (aff_entry->ae_cond == NULL
					|| PTR2CHAR(aff_entry->ae_cond) == c))
			    {
				p = aff_entry->ae_add
						  + STRLEN(aff_entry->ae_add);
				mb_ptr_back(aff_entry->ae_add, p);
				if (PTR2CHAR(p) == c_up)
				{
				    upper = TRUE;
				    aff_entry->ae_chop = NULL;
				    *p = NUL;

				    /* The condition is matched with the
				     * actual word, thus must check for the
				     * upper-case letter. */
				    if (aff_entry->ae_cond != NULL)
				    {
					char_u	buf[MAXLINELEN];
#ifdef FEAT_MBYTE
					if (has_mbyte)
					{
					    onecap_copy(items[4], buf, TRUE);
					    aff_entry->ae_cond = getroom_save(
								   spin, buf);
					}
					else
#endif
					    *aff_entry->ae_cond = c_up;
					if (aff_entry->ae_cond != NULL)
					{
					    sprintf((char *)buf, "^%s",
							  aff_entry->ae_cond);
					    vim_regfree(aff_entry->ae_prog);
					    aff_entry->ae_prog = vim_regcomp(
						    buf, RE_MAGIC + RE_STRING);
					}
				    }
				}
			    }
			}

			if (aff_entry->ae_chop == NULL
					       && aff_entry->ae_flags == NULL)
			{
			    int		idx;
			    char_u	**pp;
			    int		n;

			    /* Find a previously used condition. */
			    for (idx = spin->si_prefcond.ga_len - 1; idx >= 0;
									--idx)
			    {
				p = ((char_u **)spin->si_prefcond.ga_data)[idx];
				if (str_equal(p, aff_entry->ae_cond))
				    break;
			    }
			    if (idx < 0 && ga_grow(&spin->si_prefcond, 1) == OK)
			    {
				/* Not found, add a new condition. */
				idx = spin->si_prefcond.ga_len++;
				pp = ((char_u **)spin->si_prefcond.ga_data)
									+ idx;
				if (aff_entry->ae_cond == NULL)
				    *pp = NULL;
				else
				    *pp = getroom_save(spin,
							  aff_entry->ae_cond);
			    }

			    /* Add the prefix to the prefix tree. */
			    if (aff_entry->ae_add == NULL)
				p = (char_u *)"";
			    else
				p = aff_entry->ae_add;

			    /* PFX_FLAGS is a negative number, so that
			     * tree_add_word() knows this is the prefix tree. */
			    n = PFX_FLAGS;
			    if (!cur_aff->ah_combine)
				n |= WFP_NC;
			    if (upper)
				n |= WFP_UP;
			    if (aff_entry->ae_comppermit)
				n |= WFP_COMPPERMIT;
			    if (aff_entry->ae_compforbid)
				n |= WFP_COMPFORBID;
			    tree_add_word(spin, p, spin->si_prefroot, n,
						      idx, cur_aff->ah_newID);
			    did_postpone_prefix = TRUE;
			}

			/* Didn't actually use ah_newID, backup si_newprefID. */
			if (aff_todo == 0 && !did_postpone_prefix)
			{
			    --spin->si_newprefID;
			    cur_aff->ah_newID = 0;
			}
		    }
		}
	    }
	    else if (is_aff_rule(items, itemcnt, "FOL", 2) && fol == NULL)
	    {
		fol = vim_strsave(items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "LOW", 2) && low == NULL)
	    {
		low = vim_strsave(items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "UPP", 2) && upp == NULL)
	    {
		upp = vim_strsave(items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "REP", 2)
		     || is_aff_rule(items, itemcnt, "REPSAL", 2))
	    {
		/* Ignore REP/REPSAL count */;
		if (!isdigit(*items[1]))
		    smsg((char_u *)_("Expected REP(SAL) count in %s line %d"),
								 fname, lnum);
	    }
	    else if ((STRCMP(items[0], "REP") == 0
			|| STRCMP(items[0], "REPSAL") == 0)
		    && itemcnt >= 3)
	    {
		/* REP/REPSAL item */
		/* Myspell ignores extra arguments, we require it starts with
		 * # to detect mistakes. */
		if (itemcnt > 3 && items[3][0] != '#')
		    smsg((char_u *)_(e_afftrailing), fname, lnum, items[3]);
		if (items[0][3] == 'S' ? do_repsal : do_rep)
		{
		    /* Replace underscore with space (can't include a space
		     * directly). */
		    for (p = items[1]; *p != NUL; mb_ptr_adv(p))
			if (*p == '_')
			    *p = ' ';
		    for (p = items[2]; *p != NUL; mb_ptr_adv(p))
			if (*p == '_')
			    *p = ' ';
		    add_fromto(spin, items[0][3] == 'S'
					 ? &spin->si_repsal
					 : &spin->si_rep, items[1], items[2]);
		}
	    }
	    else if (is_aff_rule(items, itemcnt, "MAP", 2))
	    {
		/* MAP item or count */
		if (!found_map)
		{
		    /* First line contains the count. */
		    found_map = TRUE;
		    if (!isdigit(*items[1]))
			smsg((char_u *)_("Expected MAP count in %s line %d"),
								 fname, lnum);
		}
		else if (do_mapline)
		{
		    int		c;

		    /* Check that every character appears only once. */
		    for (p = items[1]; *p != NUL; )
		    {
#ifdef FEAT_MBYTE
			c = mb_ptr2char_adv(&p);
#else
			c = *p++;
#endif
			if ((spin->si_map.ga_len > 0
				    && vim_strchr(spin->si_map.ga_data, c)
								      != NULL)
				|| vim_strchr(p, c) != NULL)
			    smsg((char_u *)_("Duplicate character in MAP in %s line %d"),
								 fname, lnum);
		    }

		    /* We simply concatenate all the MAP strings, separated by
		     * slashes. */
		    ga_concat(&spin->si_map, items[1]);
		    ga_append(&spin->si_map, '/');
		}
	    }
	    /* Accept "SAL from to" and "SAL from to  #comment". */
	    else if (is_aff_rule(items, itemcnt, "SAL", 3))
	    {
		if (do_sal)
		{
		    /* SAL item (sounds-a-like)
		     * Either one of the known keys or a from-to pair. */
		    if (STRCMP(items[1], "followup") == 0)
			spin->si_followup = sal_to_bool(items[2]);
		    else if (STRCMP(items[1], "collapse_result") == 0)
			spin->si_collapse = sal_to_bool(items[2]);
		    else if (STRCMP(items[1], "remove_accents") == 0)
			spin->si_rem_accents = sal_to_bool(items[2]);
		    else
			/* when "to" is "_" it means empty */
			add_fromto(spin, &spin->si_sal, items[1],
				     STRCMP(items[2], "_") == 0 ? (char_u *)""
								: items[2]);
		}
	    }
	    else if (is_aff_rule(items, itemcnt, "SOFOFROM", 2)
							  && sofofrom == NULL)
	    {
		sofofrom = getroom_save(spin, items[1]);
	    }
	    else if (is_aff_rule(items, itemcnt, "SOFOTO", 2)
							    && sofoto == NULL)
	    {
		sofoto = getroom_save(spin, items[1]);
	    }
	    else if (STRCMP(items[0], "COMMON") == 0)
	    {
		int	i;

		for (i = 1; i < itemcnt; ++i)
		{
		    if (HASHITEM_EMPTY(hash_find(&spin->si_commonwords,
								   items[i])))
		    {
			p = vim_strsave(items[i]);
			if (p == NULL)
			    break;
			hash_add(&spin->si_commonwords, p);
		    }
		}
	    }
	    else
		smsg((char_u *)_("Unrecognized or duplicate item in %s line %d: %s"),
						       fname, lnum, items[0]);
	}
    }

    if (fol != NULL || low != NULL || upp != NULL)
    {
	if (spin->si_clear_chartab)
	{
	    /* Clear the char type tables, don't want to use any of the
	     * currently used spell properties. */
	    init_spell_chartab();
	    spin->si_clear_chartab = FALSE;
	}

	/*
	 * Don't write a word table for an ASCII file, so that we don't check
	 * for conflicts with a word table that matches 'encoding'.
	 * Don't write one for utf-8 either, we use utf_*() and
	 * mb_get_class(), the list of chars in the file will be incomplete.
	 */
	if (!spin->si_ascii
#ifdef FEAT_MBYTE
		&& !enc_utf8
#endif
		)
	{
	    if (fol == NULL || low == NULL || upp == NULL)
		smsg((char_u *)_("Missing FOL/LOW/UPP line in %s"), fname);
	    else
		(void)set_spell_chartab(fol, low, upp);
	}

	vim_free(fol);
	vim_free(low);
	vim_free(upp);
    }

    /* Use compound specifications of the .aff file for the spell info. */
    if (compmax != 0)
    {
	aff_check_number(spin->si_compmax, compmax, "COMPOUNDWORDMAX");
	spin->si_compmax = compmax;
    }

    if (compminlen != 0)
    {
	aff_check_number(spin->si_compminlen, compminlen, "COMPOUNDMIN");
	spin->si_compminlen = compminlen;
    }

    if (compsylmax != 0)
    {
	if (syllable == NULL)
	    smsg((char_u *)_("COMPOUNDSYLMAX used without SYLLABLE"));
	aff_check_number(spin->si_compsylmax, compsylmax, "COMPOUNDSYLMAX");
	spin->si_compsylmax = compsylmax;
    }

    if (compoptions != 0)
    {
	aff_check_number(spin->si_compoptions, compoptions, "COMPOUND options");
	spin->si_compoptions |= compoptions;
    }

    if (compflags != NULL)
	process_compflags(spin, aff, compflags);

    /* Check that we didn't use too many renumbered flags. */
    if (spin->si_newcompID < spin->si_newprefID)
    {
	if (spin->si_newcompID == 127 || spin->si_newcompID == 255)
	    MSG(_("Too many postponed prefixes"));
	else if (spin->si_newprefID == 0 || spin->si_newprefID == 127)
	    MSG(_("Too many compound flags"));
	else
	    MSG(_("Too many postponed prefixes and/or compound flags"));
    }

    if (syllable != NULL)
    {
	aff_check_string(spin->si_syllable, syllable, "SYLLABLE");
	spin->si_syllable = syllable;
    }

    if (sofofrom != NULL || sofoto != NULL)
    {
	if (sofofrom == NULL || sofoto == NULL)
	    smsg((char_u *)_("Missing SOFO%s line in %s"),
				     sofofrom == NULL ? "FROM" : "TO", fname);
	else if (spin->si_sal.ga_len > 0)
	    smsg((char_u *)_("Both SAL and SOFO lines in %s"), fname);
	else
	{
	    aff_check_string(spin->si_sofofr, sofofrom, "SOFOFROM");
	    aff_check_string(spin->si_sofoto, sofoto, "SOFOTO");
	    spin->si_sofofr = sofofrom;
	    spin->si_sofoto = sofoto;
	}
    }

    if (midword != NULL)
    {
	aff_check_string(spin->si_midword, midword, "MIDWORD");
	spin->si_midword = midword;
    }

    vim_free(pc);
    fclose(fd);
    return aff;
}