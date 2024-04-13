PJ_DEF(pj_status_t) pjmedia_sdp_neg_register_fmt_match_cb(
					const pj_str_t *fmt_name,
					pjmedia_sdp_neg_fmt_match_cb cb)
{
    struct fmt_match_cb_t *f = NULL;
    unsigned i;

    PJ_ASSERT_RETURN(fmt_name, PJ_EINVAL);

    /* Check if the callback for the format name has been registered */
    for (i = 0; i < fmt_match_cb_cnt; ++i) {
	if (pj_stricmp(fmt_name, &fmt_match_cb[i].fmt_name) == 0)
	    break;
    }

    /* Unregistration */
    
    if (cb == NULL) {
	if (i == fmt_match_cb_cnt)
	    return PJ_ENOTFOUND;

	pj_array_erase(fmt_match_cb, sizeof(fmt_match_cb[0]),
		       fmt_match_cb_cnt, i);
	fmt_match_cb_cnt--;

	return PJ_SUCCESS;
    }

    /* Registration */

    if (i < fmt_match_cb_cnt) {
	/* The same format name has been registered before */
	if (cb != fmt_match_cb[i].cb)
	    return PJ_EEXISTS;
	else
	    return PJ_SUCCESS;
    }

    if (fmt_match_cb_cnt >= PJ_ARRAY_SIZE(fmt_match_cb))
	return PJ_ETOOMANY;

    f = &fmt_match_cb[fmt_match_cb_cnt++];
    f->fmt_name = *fmt_name;
    f->cb = cb;

    return PJ_SUCCESS;
}