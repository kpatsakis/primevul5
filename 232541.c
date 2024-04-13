buf_write_bytes(struct bw_info *ip)
{
    int		wlen;
    char_u	*buf = ip->bw_buf;	/* data to write */
    int		len = ip->bw_len;	/* length of data */
#ifdef HAS_BW_FLAGS
    int		flags = ip->bw_flags;	/* extra flags */
#endif

#ifdef FEAT_MBYTE
    /*
     * Skip conversion when writing the crypt magic number or the BOM.
     */
    if (!(flags & FIO_NOCONVERT))
    {
	char_u		*p;
	unsigned	c;
	int		n;

	if (flags & FIO_UTF8)
	{
	    /*
	     * Convert latin1 in the buffer to UTF-8 in the file.
	     */
	    p = ip->bw_conv_buf;	/* translate to buffer */
	    for (wlen = 0; wlen < len; ++wlen)
		p += utf_char2bytes(buf[wlen], p);
	    buf = ip->bw_conv_buf;
	    len = (int)(p - ip->bw_conv_buf);
	}
	else if (flags & (FIO_UCS4 | FIO_UTF16 | FIO_UCS2 | FIO_LATIN1))
	{
	    /*
	     * Convert UTF-8 bytes in the buffer to UCS-2, UCS-4, UTF-16 or
	     * Latin1 chars in the file.
	     */
	    if (flags & FIO_LATIN1)
		p = buf;	/* translate in-place (can only get shorter) */
	    else
		p = ip->bw_conv_buf;	/* translate to buffer */
	    for (wlen = 0; wlen < len; wlen += n)
	    {
		if (wlen == 0 && ip->bw_restlen != 0)
		{
		    int		l;

		    /* Use remainder of previous call.  Append the start of
		     * buf[] to get a full sequence.  Might still be too
		     * short! */
		    l = CONV_RESTLEN - ip->bw_restlen;
		    if (l > len)
			l = len;
		    mch_memmove(ip->bw_rest + ip->bw_restlen, buf, (size_t)l);
		    n = utf_ptr2len_len(ip->bw_rest, ip->bw_restlen + l);
		    if (n > ip->bw_restlen + len)
		    {
			/* We have an incomplete byte sequence at the end to
			 * be written.  We can't convert it without the
			 * remaining bytes.  Keep them for the next call. */
			if (ip->bw_restlen + len > CONV_RESTLEN)
			    return FAIL;
			ip->bw_restlen += len;
			break;
		    }
		    if (n > 1)
			c = utf_ptr2char(ip->bw_rest);
		    else
			c = ip->bw_rest[0];
		    if (n >= ip->bw_restlen)
		    {
			n -= ip->bw_restlen;
			ip->bw_restlen = 0;
		    }
		    else
		    {
			ip->bw_restlen -= n;
			mch_memmove(ip->bw_rest, ip->bw_rest + n,
						      (size_t)ip->bw_restlen);
			n = 0;
		    }
		}
		else
		{
		    n = utf_ptr2len_len(buf + wlen, len - wlen);
		    if (n > len - wlen)
		    {
			/* We have an incomplete byte sequence at the end to
			 * be written.  We can't convert it without the
			 * remaining bytes.  Keep them for the next call. */
			if (len - wlen > CONV_RESTLEN)
			    return FAIL;
			ip->bw_restlen = len - wlen;
			mch_memmove(ip->bw_rest, buf + wlen,
						      (size_t)ip->bw_restlen);
			break;
		    }
		    if (n > 1)
			c = utf_ptr2char(buf + wlen);
		    else
			c = buf[wlen];
		}

		if (ucs2bytes(c, &p, flags) && !ip->bw_conv_error)
		{
		    ip->bw_conv_error = TRUE;
		    ip->bw_conv_error_lnum = ip->bw_start_lnum;
		}
		if (c == NL)
		    ++ip->bw_start_lnum;
	    }
	    if (flags & FIO_LATIN1)
		len = (int)(p - buf);
	    else
	    {
		buf = ip->bw_conv_buf;
		len = (int)(p - ip->bw_conv_buf);
	    }
	}

# ifdef WIN3264
	else if (flags & FIO_CODEPAGE)
	{
	    /*
	     * Convert UTF-8 or codepage to UCS-2 and then to MS-Windows
	     * codepage.
	     */
	    char_u	*from;
	    size_t	fromlen;
	    char_u	*to;
	    int		u8c;
	    BOOL	bad = FALSE;
	    int		needed;

	    if (ip->bw_restlen > 0)
	    {
		/* Need to concatenate the remainder of the previous call and
		 * the bytes of the current call.  Use the end of the
		 * conversion buffer for this. */
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    else
	    {
		from = buf;
		fromlen = len;
	    }

	    to = ip->bw_conv_buf;
	    if (enc_utf8)
	    {
		/* Convert from UTF-8 to UCS-2, to the start of the buffer.
		 * The buffer has been allocated to be big enough. */
		while (fromlen > 0)
		{
		    n = (int)utf_ptr2len_len(from, (int)fromlen);
		    if (n > (int)fromlen)	/* incomplete byte sequence */
			break;
		    u8c = utf_ptr2char(from);
		    *to++ = (u8c & 0xff);
		    *to++ = (u8c >> 8);
		    fromlen -= n;
		    from += n;
		}

		/* Copy remainder to ip->bw_rest[] to be used for the next
		 * call. */
		if (fromlen > CONV_RESTLEN)
		{
		    /* weird overlong sequence */
		    ip->bw_conv_error = TRUE;
		    return FAIL;
		}
		mch_memmove(ip->bw_rest, from, fromlen);
		ip->bw_restlen = (int)fromlen;
	    }
	    else
	    {
		/* Convert from enc_codepage to UCS-2, to the start of the
		 * buffer.  The buffer has been allocated to be big enough. */
		ip->bw_restlen = 0;
		needed = MultiByteToWideChar(enc_codepage,
			     MB_ERR_INVALID_CHARS, (LPCSTR)from, (int)fromlen,
								     NULL, 0);
		if (needed == 0)
		{
		    /* When conversion fails there may be a trailing byte. */
		    needed = MultiByteToWideChar(enc_codepage,
			 MB_ERR_INVALID_CHARS, (LPCSTR)from, (int)fromlen - 1,
								     NULL, 0);
		    if (needed == 0)
		    {
			/* Conversion doesn't work. */
			ip->bw_conv_error = TRUE;
			return FAIL;
		    }
		    /* Save the trailing byte for the next call. */
		    ip->bw_rest[0] = from[fromlen - 1];
		    ip->bw_restlen = 1;
		}
		needed = MultiByteToWideChar(enc_codepage, MB_ERR_INVALID_CHARS,
				(LPCSTR)from, (int)(fromlen - ip->bw_restlen),
							  (LPWSTR)to, needed);
		if (needed == 0)
		{
		    /* Safety check: Conversion doesn't work. */
		    ip->bw_conv_error = TRUE;
		    return FAIL;
		}
		to += needed * 2;
	    }

	    fromlen = to - ip->bw_conv_buf;
	    buf = to;
#  ifdef CP_UTF8	/* VC 4.1 doesn't define CP_UTF8 */
	    if (FIO_GET_CP(flags) == CP_UTF8)
	    {
		/* Convert from UCS-2 to UTF-8, using the remainder of the
		 * conversion buffer.  Fails when out of space. */
		for (from = ip->bw_conv_buf; fromlen > 1; fromlen -= 2)
		{
		    u8c = *from++;
		    u8c += (*from++ << 8);
		    to += utf_char2bytes(u8c, to);
		    if (to + 6 >= ip->bw_conv_buf + ip->bw_conv_buflen)
		    {
			ip->bw_conv_error = TRUE;
			return FAIL;
		    }
		}
		len = (int)(to - buf);
	    }
	    else
#endif
	    {
		/* Convert from UCS-2 to the codepage, using the remainder of
		 * the conversion buffer.  If the conversion uses the default
		 * character "0", the data doesn't fit in this encoding, so
		 * fail. */
		len = WideCharToMultiByte(FIO_GET_CP(flags), 0,
			(LPCWSTR)ip->bw_conv_buf, (int)fromlen / sizeof(WCHAR),
			(LPSTR)to, (int)(ip->bw_conv_buflen - fromlen), 0,
									&bad);
		if (bad)
		{
		    ip->bw_conv_error = TRUE;
		    return FAIL;
		}
	    }
	}
# endif

# ifdef MACOS_CONVERT
	else if (flags & FIO_MACROMAN)
	{
	    /*
	     * Convert UTF-8 or latin1 to Apple MacRoman.
	     */
	    char_u	*from;
	    size_t	fromlen;

	    if (ip->bw_restlen > 0)
	    {
		/* Need to concatenate the remainder of the previous call and
		 * the bytes of the current call.  Use the end of the
		 * conversion buffer for this. */
		fromlen = len + ip->bw_restlen;
		from = ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(from, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(from + ip->bw_restlen, buf, (size_t)len);
	    }
	    else
	    {
		from = buf;
		fromlen = len;
	    }

	    if (enc2macroman(from, fromlen,
			ip->bw_conv_buf, &len, ip->bw_conv_buflen,
			ip->bw_rest, &ip->bw_restlen) == FAIL)
	    {
		ip->bw_conv_error = TRUE;
		return FAIL;
	    }
	    buf = ip->bw_conv_buf;
	}
# endif

# ifdef USE_ICONV
	if (ip->bw_iconv_fd != (iconv_t)-1)
	{
	    const char	*from;
	    size_t	fromlen;
	    char	*to;
	    size_t	tolen;

	    /* Convert with iconv(). */
	    if (ip->bw_restlen > 0)
	    {
		char *fp;

		/* Need to concatenate the remainder of the previous call and
		 * the bytes of the current call.  Use the end of the
		 * conversion buffer for this. */
		fromlen = len + ip->bw_restlen;
		fp = (char *)ip->bw_conv_buf + ip->bw_conv_buflen - fromlen;
		mch_memmove(fp, ip->bw_rest, (size_t)ip->bw_restlen);
		mch_memmove(fp + ip->bw_restlen, buf, (size_t)len);
		from = fp;
		tolen = ip->bw_conv_buflen - fromlen;
	    }
	    else
	    {
		from = (const char *)buf;
		fromlen = len;
		tolen = ip->bw_conv_buflen;
	    }
	    to = (char *)ip->bw_conv_buf;

	    if (ip->bw_first)
	    {
		size_t	save_len = tolen;

		/* output the initial shift state sequence */
		(void)iconv(ip->bw_iconv_fd, NULL, NULL, &to, &tolen);

		/* There is a bug in iconv() on Linux (which appears to be
		 * wide-spread) which sets "to" to NULL and messes up "tolen".
		 */
		if (to == NULL)
		{
		    to = (char *)ip->bw_conv_buf;
		    tolen = save_len;
		}
		ip->bw_first = FALSE;
	    }

	    /*
	     * If iconv() has an error or there is not enough room, fail.
	     */
	    if ((iconv(ip->bw_iconv_fd, (void *)&from, &fromlen, &to, &tolen)
			== (size_t)-1 && ICONV_ERRNO != ICONV_EINVAL)
						    || fromlen > CONV_RESTLEN)
	    {
		ip->bw_conv_error = TRUE;
		return FAIL;
	    }

	    /* copy remainder to ip->bw_rest[] to be used for the next call. */
	    if (fromlen > 0)
		mch_memmove(ip->bw_rest, (void *)from, fromlen);
	    ip->bw_restlen = (int)fromlen;

	    buf = ip->bw_conv_buf;
	    len = (int)((char_u *)to - ip->bw_conv_buf);
	}
# endif
    }
#endif /* FEAT_MBYTE */

    if (ip->bw_fd < 0)
	/* Only checking conversion, which is OK if we get here. */
	return OK;

#ifdef FEAT_CRYPT
    if (flags & FIO_ENCRYPTED)
    {
	/* Encrypt the data. Do it in-place if possible, otherwise use an
	 * allocated buffer. */
	if (crypt_works_inplace(ip->bw_buffer->b_cryptstate))
	{
	    crypt_encode_inplace(ip->bw_buffer->b_cryptstate, buf, len);
	}
	else
	{
	    char_u *outbuf;

	    len = crypt_encode_alloc(curbuf->b_cryptstate, buf, len, &outbuf);
	    if (len == 0)
		return OK;  /* Crypt layer is buffering, will flush later. */
	    wlen = write_eintr(ip->bw_fd, outbuf, len);
	    vim_free(outbuf);
	    return (wlen < len) ? FAIL : OK;
	}
    }
#endif

    wlen = write_eintr(ip->bw_fd, buf, len);
    return (wlen < len) ? FAIL : OK;
}