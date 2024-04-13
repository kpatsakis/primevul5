check_for_cryptkey(
    char_u	*cryptkey,	/* previous encryption key or NULL */
    char_u	*ptr,		/* pointer to read bytes */
    long	*sizep,		/* length of read bytes */
    off_T	*filesizep,	/* nr of bytes used from file */
    int		newfile,	/* editing a new buffer */
    char_u	*fname,		/* file name to display */
    int		*did_ask)	/* flag: whether already asked for key */
{
    int method = crypt_method_nr_from_magic((char *)ptr, *sizep);
    int b_p_ro = curbuf->b_p_ro;

    if (method >= 0)
    {
	/* Mark the buffer as read-only until the decryption has taken place.
	 * Avoids accidentally overwriting the file with garbage. */
	curbuf->b_p_ro = TRUE;

	/* Set the cryptmethod local to the buffer. */
	crypt_set_cm_option(curbuf, method);
	if (cryptkey == NULL && !*did_ask)
	{
	    if (*curbuf->b_p_key)
		cryptkey = curbuf->b_p_key;
	    else
	    {
		/* When newfile is TRUE, store the typed key in the 'key'
		 * option and don't free it.  bf needs hash of the key saved.
		 * Don't ask for the key again when first time Enter was hit.
		 * Happens when retrying to detect encoding. */
		smsg((char_u *)_(need_key_msg), fname);
		msg_scroll = TRUE;
		crypt_check_method(method);
		cryptkey = crypt_get_key(newfile, FALSE);
		*did_ask = TRUE;

		/* check if empty key entered */
		if (cryptkey != NULL && *cryptkey == NUL)
		{
		    if (cryptkey != curbuf->b_p_key)
			vim_free(cryptkey);
		    cryptkey = NULL;
		}
	    }
	}

	if (cryptkey != NULL)
	{
	    int header_len;

	    curbuf->b_cryptstate = crypt_create_from_header(
						       method, cryptkey, ptr);
	    crypt_set_cm_option(curbuf, method);

	    /* Remove cryptmethod specific header from the text. */
	    header_len = crypt_get_header_len(method);
	    if (*sizep <= header_len)
		/* invalid header, buffer can't be encrypted */
		return NULL;
	    *filesizep += header_len;
	    *sizep -= header_len;
	    mch_memmove(ptr, ptr + header_len, (size_t)*sizep);

	    /* Restore the read-only flag. */
	    curbuf->b_p_ro = b_p_ro;
	}
    }
    /* When starting to edit a new file which does not have encryption, clear
     * the 'key' option, except when starting up (called with -x argument) */
    else if (newfile && *curbuf->b_p_key != NUL && !starting)
	set_option_value((char_u *)"key", 0L, (char_u *)"", OPT_LOCAL);

    return cryptkey;
}