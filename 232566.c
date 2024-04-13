next_fenc(char_u **pp)
{
    char_u	*p;
    char_u	*r;

    if (**pp == NUL)
    {
	*pp = NULL;
	return (char_u *)"";
    }
    p = vim_strchr(*pp, ',');
    if (p == NULL)
    {
	r = enc_canonize(*pp);
	*pp += STRLEN(*pp);
    }
    else
    {
	r = vim_strnsave(*pp, (int)(p - *pp));
	*pp = p + 1;
	if (r != NULL)
	{
	    p = enc_canonize(r);
	    vim_free(r);
	    r = p;
	}
    }
    if (r == NULL)	/* out of memory */
    {
	r = (char_u *)"";
	*pp = NULL;
    }
    return r;
}