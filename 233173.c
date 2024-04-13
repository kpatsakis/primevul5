spell_read_wordfile(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    long	lnum = 0;
    char_u	rline[MAXLINELEN];
    char_u	*line;
    char_u	*pc = NULL;
    char_u	*p;
    int		l;
    int		retval = OK;
    int		did_word = FALSE;
    int		non_ascii = 0;
    int		flags;
    int		regionmask;

    /*
     * Open the file.
     */
    fd = mch_fopen((char *)fname, "r");
    if (fd == NULL)
    {
	EMSG2(_(e_notopen), fname);
	return FAIL;
    }

    vim_snprintf((char *)IObuff, IOSIZE, _("Reading word file %s ..."), fname);
    spell_message(spin, IObuff);

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

	/* Remove CR, LF and white space from the end. */
	l = (int)STRLEN(rline);
	while (l > 0 && rline[l - 1] <= ' ')
	    --l;
	if (l == 0)
	    continue;	/* empty or blank line */
	rline[l] = NUL;

	/* Convert from "/encoding={encoding}" to 'encoding' when needed. */
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

	if (*line == '/')
	{
	    ++line;
	    if (STRNCMP(line, "encoding=", 9) == 0)
	    {
		if (spin->si_conv.vc_type != CONV_NONE)
		    smsg((char_u *)_("Duplicate /encoding= line ignored in %s line %d: %s"),
						       fname, lnum, line - 1);
		else if (did_word)
		    smsg((char_u *)_("/encoding= line after word ignored in %s line %d: %s"),
						       fname, lnum, line - 1);
		else
		{
#ifdef FEAT_MBYTE
		    char_u	*enc;

		    /* Setup for conversion to 'encoding'. */
		    line += 9;
		    enc = enc_canonize(line);
		    if (enc != NULL && !spin->si_ascii
			    && convert_setup(&spin->si_conv, enc,
							       p_enc) == FAIL)
			smsg((char_u *)_("Conversion in %s not supported: from %s to %s"),
							  fname, line, p_enc);
		    vim_free(enc);
		    spin->si_conv.vc_fail = TRUE;
#else
		    smsg((char_u *)_("Conversion in %s not supported"), fname);
#endif
		}
		continue;
	    }

	    if (STRNCMP(line, "regions=", 8) == 0)
	    {
		if (spin->si_region_count > 1)
		    smsg((char_u *)_("Duplicate /regions= line ignored in %s line %d: %s"),
						       fname, lnum, line);
		else
		{
		    line += 8;
		    if (STRLEN(line) > 16)
			smsg((char_u *)_("Too many regions in %s line %d: %s"),
						       fname, lnum, line);
		    else
		    {
			spin->si_region_count = (int)STRLEN(line) / 2;
			STRCPY(spin->si_region_name, line);

			/* Adjust the mask for a word valid in all regions. */
			spin->si_region = (1 << spin->si_region_count) - 1;
		    }
		}
		continue;
	    }

	    smsg((char_u *)_("/ line ignored in %s line %d: %s"),
						       fname, lnum, line - 1);
	    continue;
	}

	flags = 0;
	regionmask = spin->si_region;

	/* Check for flags and region after a slash. */
	p = vim_strchr(line, '/');
	if (p != NULL)
	{
	    *p++ = NUL;
	    while (*p != NUL)
	    {
		if (*p == '=')		/* keep-case word */
		    flags |= WF_KEEPCAP | WF_FIXCAP;
		else if (*p == '!')	/* Bad, bad, wicked word. */
		    flags |= WF_BANNED;
		else if (*p == '?')	/* Rare word. */
		    flags |= WF_RARE;
		else if (VIM_ISDIGIT(*p)) /* region number(s) */
		{
		    if ((flags & WF_REGION) == 0)   /* first one */
			regionmask = 0;
		    flags |= WF_REGION;

		    l = *p - '0';
		    if (l > spin->si_region_count)
		    {
			smsg((char_u *)_("Invalid region nr in %s line %d: %s"),
							  fname, lnum, p);
			break;
		    }
		    regionmask |= 1 << (l - 1);
		}
		else
		{
		    smsg((char_u *)_("Unrecognized flags in %s line %d: %s"),
							      fname, lnum, p);
		    break;
		}
		++p;
	    }
	}

	/* Skip non-ASCII words when "spin->si_ascii" is TRUE. */
	if (spin->si_ascii && has_non_ascii(line))
	{
	    ++non_ascii;
	    continue;
	}

	/* Normal word: store it. */
	if (store_word(spin, line, flags, regionmask, NULL, FALSE) == FAIL)
	{
	    retval = FAIL;
	    break;
	}
	did_word = TRUE;
    }

    vim_free(pc);
    fclose(fd);

    if (spin->si_ascii && non_ascii > 0)
    {
	vim_snprintf((char *)IObuff, IOSIZE,
		  _("Ignored %d words with non-ASCII characters"), non_ascii);
	spell_message(spin, IObuff);
    }

    return retval;
}