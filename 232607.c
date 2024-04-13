match_file_list(char_u *list, char_u *sfname, char_u *ffname)
{
    char_u	buf[100];
    char_u	*tail;
    char_u	*regpat;
    char	allow_dirs;
    int		match;
    char_u	*p;

    tail = gettail(sfname);

    /* try all patterns in 'wildignore' */
    p = list;
    while (*p)
    {
	copy_option_part(&p, buf, 100, ",");
	regpat = file_pat_to_reg_pat(buf, NULL, &allow_dirs, FALSE);
	if (regpat == NULL)
	    break;
	match = match_file_pat(regpat, NULL, ffname, sfname,
						       tail, (int)allow_dirs);
	vim_free(regpat);
	if (match)
	    return TRUE;
    }
    return FALSE;
}