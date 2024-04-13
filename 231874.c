static pj_status_t custom_fmt_match(pj_pool_t *pool,
				    const pj_str_t *fmt_name,
				    pjmedia_sdp_media *offer,
				    unsigned o_fmt_idx,
				    pjmedia_sdp_media *answer,
				    unsigned a_fmt_idx,
				    unsigned option)
{
    unsigned i;

    for (i = 0; i < fmt_match_cb_cnt; ++i) {
	if (pj_stricmp(fmt_name, &fmt_match_cb[i].fmt_name) == 0) {
	    pj_assert(fmt_match_cb[i].cb);
	    return (*fmt_match_cb[i].cb)(pool, offer, o_fmt_idx,
					 answer, a_fmt_idx,
					 option);
	}
    }

    /* Not customized format matching found, should be matched */
    return PJ_SUCCESS;
}