free_all_autocmds(void)
{
    int		i;
    char_u	*s;

    for (current_augroup = -1; current_augroup < augroups.ga_len;
							    ++current_augroup)
	do_autocmd((char_u *)"", TRUE);

    for (i = 0; i < augroups.ga_len; ++i)
    {
	s = ((char_u **)(augroups.ga_data))[i];
	if (s != get_deleted_augroup())
	    vim_free(s);
    }
    ga_clear(&augroups);
}