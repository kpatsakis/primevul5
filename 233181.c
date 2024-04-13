init_spellfile(void)
{
    char_u	*buf;
    int		l;
    char_u	*fname;
    char_u	*rtp;
    char_u	*lend;
    int		aspath = FALSE;
    char_u	*lstart = curbuf->b_s.b_p_spl;

    if (*curwin->w_s->b_p_spl != NUL && curwin->w_s->b_langp.ga_len > 0)
    {
	buf = alloc(MAXPATHL);
	if (buf == NULL)
	    return;

	/* Find the end of the language name.  Exclude the region.  If there
	 * is a path separator remember the start of the tail. */
	for (lend = curwin->w_s->b_p_spl; *lend != NUL
			&& vim_strchr((char_u *)",._", *lend) == NULL; ++lend)
	    if (vim_ispathsep(*lend))
	    {
		aspath = TRUE;
		lstart = lend + 1;
	    }

	/* Loop over all entries in 'runtimepath'.  Use the first one where we
	 * are allowed to write. */
	rtp = p_rtp;
	while (*rtp != NUL)
	{
	    if (aspath)
		/* Use directory of an entry with path, e.g., for
		 * "/dir/lg.utf-8.spl" use "/dir". */
		vim_strncpy(buf, curbuf->b_s.b_p_spl,
					    lstart - curbuf->b_s.b_p_spl - 1);
	    else
		/* Copy the path from 'runtimepath' to buf[]. */
		copy_option_part(&rtp, buf, MAXPATHL, ",");
	    if (filewritable(buf) == 2)
	    {
		/* Use the first language name from 'spelllang' and the
		 * encoding used in the first loaded .spl file. */
		if (aspath)
		    vim_strncpy(buf, curbuf->b_s.b_p_spl,
						  lend - curbuf->b_s.b_p_spl);
		else
		{
		    /* Create the "spell" directory if it doesn't exist yet. */
		    l = (int)STRLEN(buf);
		    vim_snprintf((char *)buf + l, MAXPATHL - l, "/spell");
		    if (filewritable(buf) != 2)
			vim_mkdir(buf, 0755);

		    l = (int)STRLEN(buf);
		    vim_snprintf((char *)buf + l, MAXPATHL - l,
				 "/%.*s", (int)(lend - lstart), lstart);
		}
		l = (int)STRLEN(buf);
		fname = LANGP_ENTRY(curwin->w_s->b_langp, 0)
							 ->lp_slang->sl_fname;
		vim_snprintf((char *)buf + l, MAXPATHL - l, ".%s.add",
			fname != NULL
			  && strstr((char *)gettail(fname), ".ascii.") != NULL
				       ? (char_u *)"ascii" : spell_enc());
		set_option_value((char_u *)"spellfile", 0L, buf, OPT_LOCAL);
		break;
	    }
	    aspath = FALSE;
	}

	vim_free(buf);
    }
}