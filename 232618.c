shorten_fname(char_u *full_path, char_u *dir_name)
{
    int		len;
    char_u	*p;

    if (full_path == NULL)
	return NULL;
    len = (int)STRLEN(dir_name);
    if (fnamencmp(dir_name, full_path, len) == 0)
    {
	p = full_path + len;
#if defined(MSWIN)
	/*
	 * MSWIN: when a file is in the root directory, dir_name will end in a
	 * slash, since C: by itself does not define a specific dir. In this
	 * case p may already be correct. <negri>
	 */
	if (!((len > 2) && (*(p - 2) == ':')))
#endif
	{
	    if (vim_ispathsep(*p))
		++p;
#ifndef VMS   /* the path separator is always part of the path */
	    else
		p = NULL;
#endif
	}
    }
#if defined(MSWIN)
    /*
     * When using a file in the current drive, remove the drive name:
     * "A:\dir\file" -> "\dir\file".  This helps when moving a session file on
     * a floppy from "A:\dir" to "B:\dir".
     */
    else if (len > 3
	    && TOUPPER_LOC(full_path[0]) == TOUPPER_LOC(dir_name[0])
	    && full_path[1] == ':'
	    && vim_ispathsep(full_path[2]))
	p = full_path + 2;
#endif
    else
	p = NULL;
    return p;
}