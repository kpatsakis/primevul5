invalid_range(exarg_T *eap)
{
    buf_T	*buf;

    if (       eap->line1 < 0
	    || eap->line2 < 0
	    || eap->line1 > eap->line2)
	return _(e_invalid_range);

    if (eap->argt & EX_RANGE)
    {
	switch (eap->addr_type)
	{
	    case ADDR_LINES:
		if (eap->line2 > curbuf->b_ml.ml_line_count
#ifdef FEAT_DIFF
			    + (eap->cmdidx == CMD_diffget)
#endif
		   )
		    return _(e_invalid_range);
		break;
	    case ADDR_ARGUMENTS:
		// add 1 if ARGCOUNT is 0
		if (eap->line2 > ARGCOUNT + (!ARGCOUNT))
		    return _(e_invalid_range);
		break;
	    case ADDR_BUFFERS:
		// Only a boundary check, not whether the buffers actually
		// exist.
		if (eap->line1 < 1 || eap->line2 > get_highest_fnum())
		    return _(e_invalid_range);
		break;
	    case ADDR_LOADED_BUFFERS:
		buf = firstbuf;
		while (buf->b_ml.ml_mfp == NULL)
		{
		    if (buf->b_next == NULL)
			return _(e_invalid_range);
		    buf = buf->b_next;
		}
		if (eap->line1 < buf->b_fnum)
		    return _(e_invalid_range);
		buf = lastbuf;
		while (buf->b_ml.ml_mfp == NULL)
		{
		    if (buf->b_prev == NULL)
			return _(e_invalid_range);
		    buf = buf->b_prev;
		}
		if (eap->line2 > buf->b_fnum)
		    return _(e_invalid_range);
		break;
	    case ADDR_WINDOWS:
		if (eap->line2 > LAST_WIN_NR)
		    return _(e_invalid_range);
		break;
	    case ADDR_TABS:
		if (eap->line2 > LAST_TAB_NR)
		    return _(e_invalid_range);
		break;
	    case ADDR_TABS_RELATIVE:
	    case ADDR_OTHER:
		// Any range is OK.
		break;
	    case ADDR_QUICKFIX:
#ifdef FEAT_QUICKFIX
		// No error for value that is too big, will use the last entry.
		if (eap->line2 <= 0)
		    return _(e_invalid_range);
#endif
		break;
	    case ADDR_QUICKFIX_VALID:
#ifdef FEAT_QUICKFIX
		if ((eap->line2 != 1 && eap->line2 > qf_get_valid_size(eap))
			|| eap->line2 < 0)
		    return _(e_invalid_range);
#endif
		break;
	    case ADDR_UNSIGNED:
	    case ADDR_NONE:
		// Will give an error elsewhere.
		break;
	}
    }
    return NULL;
}