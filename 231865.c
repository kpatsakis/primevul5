static void apply_answer_symmetric_pt(pj_pool_t *pool,
				      pjmedia_sdp_media *answer,
				      unsigned pt_cnt,
				      const pj_str_t pt_offer[],
				      const pj_str_t pt_answer[])
{
    pjmedia_sdp_attr *a_tmp[PJMEDIA_MAX_SDP_ATTR];
    unsigned i, a_tmp_cnt = 0;

    /* Rewrite the payload types in the answer if different to
     * the ones in the offer.
     */
    for (i = 0; i < pt_cnt; ++i) {
	pjmedia_sdp_attr *a;

	/* Skip if the PTs are the same already, e.g: static PT. */
	if (pj_strcmp(&pt_answer[i], &pt_offer[i]) == 0)
	    continue;

	/* Rewrite payload type in the answer to match to the offer */
	pj_strdup(pool, &answer->desc.fmt[i], &pt_offer[i]);

	/* Also update payload type in rtpmap */
	a = pjmedia_sdp_media_find_attr2(answer, "rtpmap", &pt_answer[i]);
	if (a) {
	    rewrite_pt(pool, &a->value, &pt_answer[i], &pt_offer[i]);
	    /* Temporarily remove the attribute in case the new payload
	     * type is being used by another format in the media.
	     */
	    pjmedia_sdp_media_remove_attr(answer, a);
	    a_tmp[a_tmp_cnt++] = a;
	}

	/* Also update payload type in fmtp */
	a = pjmedia_sdp_media_find_attr2(answer, "fmtp", &pt_answer[i]);
	if (a) {
	    rewrite_pt(pool, &a->value, &pt_answer[i], &pt_offer[i]);
	    /* Temporarily remove the attribute in case the new payload
	     * type is being used by another format in the media.
	     */
	    pjmedia_sdp_media_remove_attr(answer, a);
	    a_tmp[a_tmp_cnt++] = a;
	}
    }

    /* Return back 'rtpmap' and 'fmtp' attributes */
    for (i = 0; i < a_tmp_cnt; ++i)
	pjmedia_sdp_media_add_attr(answer, a_tmp[i]);
}