get_loop_line(int c, void *cookie, int indent, getline_opt_T options)
{
    struct loop_cookie	*cp = (struct loop_cookie *)cookie;
    wcmd_T		*wp;
    char_u		*line;

    if (cp->current_line + 1 >= cp->lines_gap->ga_len)
    {
	if (cp->repeating)
	    return NULL;	// trying to read past ":endwhile"/":endfor"

	// First time inside the ":while"/":for": get line normally.
	if (cp->getline == NULL)
	    line = getcmdline(c, 0L, indent, options);
	else
	    line = cp->getline(c, cp->cookie, indent, options);
	if (line != NULL && store_loop_line(cp->lines_gap, line) == OK)
	    ++cp->current_line;

	return line;
    }

    KeyTyped = FALSE;
    ++cp->current_line;
    wp = (wcmd_T *)(cp->lines_gap->ga_data) + cp->current_line;
    SOURCING_LNUM = wp->lnum;
    return vim_strsave(wp->line);
}