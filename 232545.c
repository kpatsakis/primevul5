do_autocmd_event(
    event_T	event,
    char_u	*pat,
    int		nested,
    char_u	*cmd,
    int		forceit,
    int		group)
{
    AutoPat	*ap;
    AutoPat	**prev_ap;
    AutoCmd	*ac;
    AutoCmd	**prev_ac;
    int		brace_level;
    char_u	*endpat;
    int		findgroup;
    int		allgroups;
    int		patlen;
    int		is_buflocal;
    int		buflocal_nr;
    char_u	buflocal_pat[25];	/* for "<buffer=X>" */

    if (group == AUGROUP_ALL)
	findgroup = current_augroup;
    else
	findgroup = group;
    allgroups = (group == AUGROUP_ALL && !forceit && *cmd == NUL);

    /*
     * Show or delete all patterns for an event.
     */
    if (*pat == NUL)
    {
	for (ap = first_autopat[(int)event]; ap != NULL; ap = ap->next)
	{
	    if (forceit)  /* delete the AutoPat, if it's in the current group */
	    {
		if (ap->group == findgroup)
		    au_remove_pat(ap);
	    }
	    else if (group == AUGROUP_ALL || ap->group == group)
		show_autocmd(ap, event);
	}
    }

    /*
     * Loop through all the specified patterns.
     */
    for ( ; *pat; pat = (*endpat == ',' ? endpat + 1 : endpat))
    {
	/*
	 * Find end of the pattern.
	 * Watch out for a comma in braces, like "*.\{obj,o\}".
	 */
	brace_level = 0;
	for (endpat = pat; *endpat && (*endpat != ',' || brace_level
			   || (endpat > pat && endpat[-1] == '\\')); ++endpat)
	{
	    if (*endpat == '{')
		brace_level++;
	    else if (*endpat == '}')
		brace_level--;
	}
	if (pat == endpat)		/* ignore single comma */
	    continue;
	patlen = (int)(endpat - pat);

	/*
	 * detect special <buflocal[=X]> buffer-local patterns
	 */
	is_buflocal = FALSE;
	buflocal_nr = 0;

	if (patlen >= 8 && STRNCMP(pat, "<buffer", 7) == 0
						    && pat[patlen - 1] == '>')
	{
	    /* "<buffer...>": Error will be printed only for addition.
	     * printing and removing will proceed silently. */
	    is_buflocal = TRUE;
	    if (patlen == 8)
		/* "<buffer>" */
		buflocal_nr = curbuf->b_fnum;
	    else if (patlen > 9 && pat[7] == '=')
	    {
		if (patlen == 13 && STRNICMP(pat, "<buffer=abuf>", 13) == 0)
		    /* "<buffer=abuf>" */
		    buflocal_nr = autocmd_bufnr;
		else if (skipdigits(pat + 8) == pat + patlen - 1)
		    /* "<buffer=123>" */
		    buflocal_nr = atoi((char *)pat + 8);
	    }
	}

	if (is_buflocal)
	{
	    /* normalize pat into standard "<buffer>#N" form */
	    sprintf((char *)buflocal_pat, "<buffer=%d>", buflocal_nr);
	    pat = buflocal_pat;			/* can modify pat and patlen */
	    patlen = (int)STRLEN(buflocal_pat);	/*   but not endpat */
	}

	/*
	 * Find AutoPat entries with this pattern.
	 */
	prev_ap = &first_autopat[(int)event];
	while ((ap = *prev_ap) != NULL)
	{
	    if (ap->pat != NULL)
	    {
		/* Accept a pattern when:
		 * - a group was specified and it's that group, or a group was
		 *   not specified and it's the current group, or a group was
		 *   not specified and we are listing
		 * - the length of the pattern matches
		 * - the pattern matches.
		 * For <buffer[=X]>, this condition works because we normalize
		 * all buffer-local patterns.
		 */
		if ((allgroups || ap->group == findgroup)
			&& ap->patlen == patlen
			&& STRNCMP(pat, ap->pat, patlen) == 0)
		{
		    /*
		     * Remove existing autocommands.
		     * If adding any new autocmd's for this AutoPat, don't
		     * delete the pattern from the autopat list, append to
		     * this list.
		     */
		    if (forceit)
		    {
			if (*cmd != NUL && ap->next == NULL)
			{
			    au_remove_cmds(ap);
			    break;
			}
			au_remove_pat(ap);
		    }

		    /*
		     * Show autocmd's for this autopat, or buflocals <buffer=X>
		     */
		    else if (*cmd == NUL)
			show_autocmd(ap, event);

		    /*
		     * Add autocmd to this autopat, if it's the last one.
		     */
		    else if (ap->next == NULL)
			break;
		}
	    }
	    prev_ap = &ap->next;
	}

	/*
	 * Add a new command.
	 */
	if (*cmd != NUL)
	{
	    /*
	     * If the pattern we want to add a command to does appear at the
	     * end of the list (or not is not in the list at all), add the
	     * pattern at the end of the list.
	     */
	    if (ap == NULL)
	    {
		/* refuse to add buffer-local ap if buffer number is invalid */
		if (is_buflocal && (buflocal_nr == 0
				      || buflist_findnr(buflocal_nr) == NULL))
		{
		    EMSGN(_("E680: <buffer=%d>: invalid buffer number "),
								 buflocal_nr);
		    return FAIL;
		}

		ap = (AutoPat *)alloc((unsigned)sizeof(AutoPat));
		if (ap == NULL)
		    return FAIL;
		ap->pat = vim_strnsave(pat, patlen);
		ap->patlen = patlen;
		if (ap->pat == NULL)
		{
		    vim_free(ap);
		    return FAIL;
		}

		if (is_buflocal)
		{
		    ap->buflocal_nr = buflocal_nr;
		    ap->reg_prog = NULL;
		}
		else
		{
		    char_u	*reg_pat;

		    ap->buflocal_nr = 0;
		    reg_pat = file_pat_to_reg_pat(pat, endpat,
							 &ap->allow_dirs, TRUE);
		    if (reg_pat != NULL)
			ap->reg_prog = vim_regcomp(reg_pat, RE_MAGIC);
		    vim_free(reg_pat);
		    if (reg_pat == NULL || ap->reg_prog == NULL)
		    {
			vim_free(ap->pat);
			vim_free(ap);
			return FAIL;
		    }
		}
		ap->cmds = NULL;
		*prev_ap = ap;
		ap->next = NULL;
		if (group == AUGROUP_ALL)
		    ap->group = current_augroup;
		else
		    ap->group = group;
	    }

	    /*
	     * Add the autocmd at the end of the AutoCmd list.
	     */
	    prev_ac = &(ap->cmds);
	    while ((ac = *prev_ac) != NULL)
		prev_ac = &ac->next;
	    ac = (AutoCmd *)alloc((unsigned)sizeof(AutoCmd));
	    if (ac == NULL)
		return FAIL;
	    ac->cmd = vim_strsave(cmd);
#ifdef FEAT_EVAL
	    ac->scriptID = current_SID;
#endif
	    if (ac->cmd == NULL)
	    {
		vim_free(ac);
		return FAIL;
	    }
	    ac->next = NULL;
	    *prev_ac = ac;
	    ac->nested = nested;
	}
    }

    au_cleanup();	/* may really delete removed patterns/commands now */
    return OK;
}