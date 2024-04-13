file_pat_to_reg_pat(
    char_u	*pat,
    char_u	*pat_end,	/* first char after pattern or NULL */
    char	*allow_dirs,	/* Result passed back out in here */
    int		no_bslash UNUSED) /* Don't use a backward slash as pathsep */
{
    int		size = 2; /* '^' at start, '$' at end */
    char_u	*endp;
    char_u	*reg_pat;
    char_u	*p;
    int		i;
    int		nested = 0;
    int		add_dollar = TRUE;

    if (allow_dirs != NULL)
	*allow_dirs = FALSE;
    if (pat_end == NULL)
	pat_end = pat + STRLEN(pat);

    for (p = pat; p < pat_end; p++)
    {
	switch (*p)
	{
	    case '*':
	    case '.':
	    case ',':
	    case '{':
	    case '}':
	    case '~':
		size += 2;	/* extra backslash */
		break;
#ifdef BACKSLASH_IN_FILENAME
	    case '\\':
	    case '/':
		size += 4;	/* could become "[\/]" */
		break;
#endif
	    default:
		size++;
# ifdef FEAT_MBYTE
		if (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		{
		    ++p;
		    ++size;
		}
# endif
		break;
	}
    }
    reg_pat = alloc(size + 1);
    if (reg_pat == NULL)
	return NULL;

    i = 0;

    if (pat[0] == '*')
	while (pat[0] == '*' && pat < pat_end - 1)
	    pat++;
    else
	reg_pat[i++] = '^';
    endp = pat_end - 1;
    if (endp >= pat && *endp == '*')
    {
	while (endp - pat > 0 && *endp == '*')
	    endp--;
	add_dollar = FALSE;
    }
    for (p = pat; *p && nested >= 0 && p <= endp; p++)
    {
	switch (*p)
	{
	    case '*':
		reg_pat[i++] = '.';
		reg_pat[i++] = '*';
		while (p[1] == '*')	/* "**" matches like "*" */
		    ++p;
		break;
	    case '.':
	    case '~':
		reg_pat[i++] = '\\';
		reg_pat[i++] = *p;
		break;
	    case '?':
		reg_pat[i++] = '.';
		break;
	    case '\\':
		if (p[1] == NUL)
		    break;
#ifdef BACKSLASH_IN_FILENAME
		if (!no_bslash)
		{
		    /* translate:
		     * "\x" to "\\x"  e.g., "dir\file"
		     * "\*" to "\\.*" e.g., "dir\*.c"
		     * "\?" to "\\."  e.g., "dir\??.c"
		     * "\+" to "\+"   e.g., "fileX\+.c"
		     */
		    if ((vim_isfilec(p[1]) || p[1] == '*' || p[1] == '?')
			    && p[1] != '+')
		    {
			reg_pat[i++] = '[';
			reg_pat[i++] = '\\';
			reg_pat[i++] = '/';
			reg_pat[i++] = ']';
			if (allow_dirs != NULL)
			    *allow_dirs = TRUE;
			break;
		    }
		}
#endif
		/* Undo escaping from ExpandEscape():
		 * foo\?bar -> foo?bar
		 * foo\%bar -> foo%bar
		 * foo\,bar -> foo,bar
		 * foo\ bar -> foo bar
		 * Don't unescape \, * and others that are also special in a
		 * regexp.
		 * An escaped { must be unescaped since we use magic not
		 * verymagic.  Use "\\\{n,m\}"" to get "\{n,m}".
		 */
		if (*++p == '?'
#ifdef BACKSLASH_IN_FILENAME
			&& no_bslash
#endif
			)
		    reg_pat[i++] = '?';
		else
		    if (*p == ',' || *p == '%' || *p == '#'
			       || vim_isspace(*p) || *p == '{' || *p == '}')
			reg_pat[i++] = *p;
		    else if (*p == '\\' && p[1] == '\\' && p[2] == '{')
		    {
			reg_pat[i++] = '\\';
			reg_pat[i++] = '{';
			p += 2;
		    }
		    else
		    {
			if (allow_dirs != NULL && vim_ispathsep(*p)
#ifdef BACKSLASH_IN_FILENAME
				&& (!no_bslash || *p != '\\')
#endif
				)
			    *allow_dirs = TRUE;
			reg_pat[i++] = '\\';
			reg_pat[i++] = *p;
		    }
		break;
#ifdef BACKSLASH_IN_FILENAME
	    case '/':
		reg_pat[i++] = '[';
		reg_pat[i++] = '\\';
		reg_pat[i++] = '/';
		reg_pat[i++] = ']';
		if (allow_dirs != NULL)
		    *allow_dirs = TRUE;
		break;
#endif
	    case '{':
		reg_pat[i++] = '\\';
		reg_pat[i++] = '(';
		nested++;
		break;
	    case '}':
		reg_pat[i++] = '\\';
		reg_pat[i++] = ')';
		--nested;
		break;
	    case ',':
		if (nested)
		{
		    reg_pat[i++] = '\\';
		    reg_pat[i++] = '|';
		}
		else
		    reg_pat[i++] = ',';
		break;
	    default:
# ifdef  FEAT_MBYTE
		if (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
		    reg_pat[i++] = *p++;
		else
# endif
		if (allow_dirs != NULL && vim_ispathsep(*p))
		    *allow_dirs = TRUE;
		reg_pat[i++] = *p;
		break;
	}
    }
    if (add_dollar)
	reg_pat[i++] = '$';
    reg_pat[i] = NUL;
    if (nested != 0)
    {
	if (nested < 0)
	    EMSG(_("E219: Missing {."));
	else
	    EMSG(_("E220: Missing }."));
	vim_free(reg_pat);
	reg_pat = NULL;
    }
    return reg_pat;
}