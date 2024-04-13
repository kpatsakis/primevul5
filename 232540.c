prep_exarg(exarg_T *eap, buf_T *buf)
{
    eap->cmd = alloc((unsigned)(STRLEN(buf->b_p_ff)
#ifdef FEAT_MBYTE
		+ STRLEN(buf->b_p_fenc)
#endif
						 + 15));
    if (eap->cmd == NULL)
	return FAIL;

#ifdef FEAT_MBYTE
    sprintf((char *)eap->cmd, "e ++ff=%s ++enc=%s", buf->b_p_ff, buf->b_p_fenc);
    eap->force_enc = 14 + (int)STRLEN(buf->b_p_ff);
    eap->bad_char = buf->b_bad_char;
#else
    sprintf((char *)eap->cmd, "e ++ff=%s", buf->b_p_ff);
#endif
    eap->force_ff = 7;

    eap->force_bin = buf->b_p_bin ? FORCE_BIN : FORCE_NOBIN;
    eap->read_edit = FALSE;
    eap->forceit = FALSE;
    return OK;
}