store_aff_word(
    spellinfo_T	*spin,		/* spell info */
    char_u	*word,		/* basic word start */
    char_u	*afflist,	/* list of names of supported affixes */
    afffile_T	*affile,
    hashtab_T	*ht,
    hashtab_T	*xht,
    int		condit,		/* CONDIT_SUF et al. */
    int		flags,		/* flags for the word */
    char_u	*pfxlist,	/* list of prefix IDs */
    int		pfxlen)		/* nr of flags in "pfxlist" for prefixes, rest
				 * is compound flags */
{
    int		todo;
    hashitem_T	*hi;
    affheader_T	*ah;
    affentry_T	*ae;
    char_u	newword[MAXWLEN];
    int		retval = OK;
    int		i, j;
    char_u	*p;
    int		use_flags;
    char_u	*use_pfxlist;
    int		use_pfxlen;
    int		need_affix;
    char_u	store_afflist[MAXWLEN];
    char_u	pfx_pfxlist[MAXWLEN];
    size_t	wordlen = STRLEN(word);
    int		use_condit;

    todo = (int)ht->ht_used;
    for (hi = ht->ht_array; todo > 0 && retval == OK; ++hi)
    {
	if (!HASHITEM_EMPTY(hi))
	{
	    --todo;
	    ah = HI2AH(hi);

	    /* Check that the affix combines, if required, and that the word
	     * supports this affix. */
	    if (((condit & CONDIT_COMB) == 0 || ah->ah_combine)
		    && flag_in_afflist(affile->af_flagtype, afflist,
								 ah->ah_flag))
	    {
		/* Loop over all affix entries with this name. */
		for (ae = ah->ah_first; ae != NULL; ae = ae->ae_next)
		{
		    /* Check the condition.  It's not logical to match case
		     * here, but it is required for compatibility with
		     * Myspell.
		     * Another requirement from Myspell is that the chop
		     * string is shorter than the word itself.
		     * For prefixes, when "PFXPOSTPONE" was used, only do
		     * prefixes with a chop string and/or flags.
		     * When a previously added affix had CIRCUMFIX this one
		     * must have it too, if it had not then this one must not
		     * have one either. */
		    if ((xht != NULL || !affile->af_pfxpostpone
				|| ae->ae_chop != NULL
				|| ae->ae_flags != NULL)
			    && (ae->ae_chop == NULL
				|| STRLEN(ae->ae_chop) < wordlen)
			    && (ae->ae_prog == NULL
				|| vim_regexec_prog(&ae->ae_prog, FALSE,
							    word, (colnr_T)0))
			    && (((condit & CONDIT_CFIX) == 0)
				== ((condit & CONDIT_AFF) == 0
				    || ae->ae_flags == NULL
				    || !flag_in_afflist(affile->af_flagtype,
					ae->ae_flags, affile->af_circumfix))))
		    {
			/* Match.  Remove the chop and add the affix. */
			if (xht == NULL)
			{
			    /* prefix: chop/add at the start of the word */
			    if (ae->ae_add == NULL)
				*newword = NUL;
			    else
				vim_strncpy(newword, ae->ae_add, MAXWLEN - 1);
			    p = word;
			    if (ae->ae_chop != NULL)
			    {
				/* Skip chop string. */
#ifdef FEAT_MBYTE
				if (has_mbyte)
				{
				    i = mb_charlen(ae->ae_chop);
				    for ( ; i > 0; --i)
					mb_ptr_adv(p);
				}
				else
#endif
				    p += STRLEN(ae->ae_chop);
			    }
			    STRCAT(newword, p);
			}
			else
			{
			    /* suffix: chop/add at the end of the word */
			    vim_strncpy(newword, word, MAXWLEN - 1);
			    if (ae->ae_chop != NULL)
			    {
				/* Remove chop string. */
				p = newword + STRLEN(newword);
				i = (int)MB_CHARLEN(ae->ae_chop);
				for ( ; i > 0; --i)
				    mb_ptr_back(newword, p);
				*p = NUL;
			    }
			    if (ae->ae_add != NULL)
				STRCAT(newword, ae->ae_add);
			}

			use_flags = flags;
			use_pfxlist = pfxlist;
			use_pfxlen = pfxlen;
			need_affix = FALSE;
			use_condit = condit | CONDIT_COMB | CONDIT_AFF;
			if (ae->ae_flags != NULL)
			{
			    /* Extract flags from the affix list. */
			    use_flags |= get_affix_flags(affile, ae->ae_flags);

			    if (affile->af_needaffix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_needaffix))
				need_affix = TRUE;

			    /* When there is a CIRCUMFIX flag the other affix
			     * must also have it and we don't add the word
			     * with one affix. */
			    if (affile->af_circumfix != 0 && flag_in_afflist(
					affile->af_flagtype, ae->ae_flags,
							affile->af_circumfix))
			    {
				use_condit |= CONDIT_CFIX;
				if ((condit & CONDIT_CFIX) == 0)
				    need_affix = TRUE;
			    }

			    if (affile->af_pfxpostpone
						|| spin->si_compflags != NULL)
			    {
				if (affile->af_pfxpostpone)
				    /* Get prefix IDS from the affix list. */
				    use_pfxlen = get_pfxlist(affile,
						 ae->ae_flags, store_afflist);
				else
				    use_pfxlen = 0;
				use_pfxlist = store_afflist;

				/* Combine the prefix IDs. Avoid adding the
				 * same ID twice. */
				for (i = 0; i < pfxlen; ++i)
				{
				    for (j = 0; j < use_pfxlen; ++j)
					if (pfxlist[i] == use_pfxlist[j])
					    break;
				    if (j == use_pfxlen)
					use_pfxlist[use_pfxlen++] = pfxlist[i];
				}

				if (spin->si_compflags != NULL)
				    /* Get compound IDS from the affix list. */
				    get_compflags(affile, ae->ae_flags,
						  use_pfxlist + use_pfxlen);

				/* Combine the list of compound flags.
				 * Concatenate them to the prefix IDs list.
				 * Avoid adding the same ID twice. */
				for (i = pfxlen; pfxlist[i] != NUL; ++i)
				{
				    for (j = use_pfxlen;
						   use_pfxlist[j] != NUL; ++j)
					if (pfxlist[i] == use_pfxlist[j])
					    break;
				    if (use_pfxlist[j] == NUL)
				    {
					use_pfxlist[j++] = pfxlist[i];
					use_pfxlist[j] = NUL;
				    }
				}
			    }
			}

			/* Obey a "COMPOUNDFORBIDFLAG" of the affix: don't
			 * use the compound flags. */
			if (use_pfxlist != NULL && ae->ae_compforbid)
			{
			    vim_strncpy(pfx_pfxlist, use_pfxlist, use_pfxlen);
			    use_pfxlist = pfx_pfxlist;
			}

			/* When there are postponed prefixes... */
			if (spin->si_prefroot != NULL
				&& spin->si_prefroot->wn_sibling != NULL)
			{
			    /* ... add a flag to indicate an affix was used. */
			    use_flags |= WF_HAS_AFF;

			    /* ... don't use a prefix list if combining
			     * affixes is not allowed.  But do use the
			     * compound flags after them. */
			    if (!ah->ah_combine && use_pfxlist != NULL)
				use_pfxlist += use_pfxlen;
			}

			/* When compounding is supported and there is no
			 * "COMPOUNDPERMITFLAG" then forbid compounding on the
			 * side where the affix is applied. */
			if (spin->si_compflags != NULL && !ae->ae_comppermit)
			{
			    if (xht != NULL)
				use_flags |= WF_NOCOMPAFT;
			    else
				use_flags |= WF_NOCOMPBEF;
			}

			/* Store the modified word. */
			if (store_word(spin, newword, use_flags,
						 spin->si_region, use_pfxlist,
							  need_affix) == FAIL)
			    retval = FAIL;

			/* When added a prefix or a first suffix and the affix
			 * has flags may add a(nother) suffix.  RECURSIVE! */
			if ((condit & CONDIT_SUF) && ae->ae_flags != NULL)
			    if (store_aff_word(spin, newword, ae->ae_flags,
					affile, &affile->af_suff, xht,
					   use_condit & (xht == NULL
							? ~0 :  ~CONDIT_SUF),
				      use_flags, use_pfxlist, pfxlen) == FAIL)
				retval = FAIL;

			/* When added a suffix and combining is allowed also
			 * try adding a prefix additionally.  Both for the
			 * word flags and for the affix flags.  RECURSIVE! */
			if (xht != NULL && ah->ah_combine)
			{
			    if (store_aff_word(spin, newword,
					afflist, affile,
					xht, NULL, use_condit,
					use_flags, use_pfxlist,
					pfxlen) == FAIL
				    || (ae->ae_flags != NULL
					&& store_aff_word(spin, newword,
					    ae->ae_flags, affile,
					    xht, NULL, use_condit,
					    use_flags, use_pfxlist,
					    pfxlen) == FAIL))
				retval = FAIL;
			}
		    }
		}
	    }
	}
    }

    return retval;
}