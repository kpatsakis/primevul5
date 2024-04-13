forward_slash(char_u *fname)
{
    char_u	*p;

    if (path_with_url(fname))
	return;
    for (p = fname; *p != NUL; ++p)
# ifdef  FEAT_MBYTE
	/* The Big5 encoding can have '\' in the trail byte. */
	if (enc_dbcs != 0 && (*mb_ptr2len)(p) > 1)
	    ++p;
	else
# endif
	if (*p == '\\')
	    *p = '/';
}