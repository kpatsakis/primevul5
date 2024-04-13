au_event_disable(char *what)
{
    char_u	*new_ei;
    char_u	*save_ei;

    save_ei = vim_strsave(p_ei);
    if (save_ei != NULL)
    {
	new_ei = vim_strnsave(p_ei, (int)(STRLEN(p_ei) + STRLEN(what)));
	if (new_ei != NULL)
	{
	    if (*what == ',' && *p_ei == NUL)
		STRCPY(new_ei, what + 1);
	    else
		STRCAT(new_ei, what);
	    set_string_option_direct((char_u *)"ei", -1, new_ei,
							  OPT_FREE, SID_NONE);
	    vim_free(new_ei);
	}
    }
    return save_ei;
}