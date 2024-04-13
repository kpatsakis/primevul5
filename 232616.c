event_name2nr(char_u *start, char_u **end)
{
    char_u	*p;
    int		i;
    int		len;

    /* the event name ends with end of line, '|', a blank or a comma */
    for (p = start; *p && !VIM_ISWHITE(*p) && *p != ',' && *p != '|'; ++p)
	;
    for (i = 0; event_names[i].name != NULL; ++i)
    {
	len = (int)STRLEN(event_names[i].name);
	if (len == p - start && STRNICMP(event_names[i].name, start, len) == 0)
	    break;
    }
    if (*p == ',')
	++p;
    *end = p;
    if (event_names[i].name == NULL)
	return NUM_EVENTS;
    return event_names[i].event;
}