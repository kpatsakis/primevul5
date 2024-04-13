PJ_DEF(pj_status_t) pjmedia_sdp_neg_create_w_remote_offer(pj_pool_t *pool,
				      const pjmedia_sdp_session *initial,
				      const pjmedia_sdp_session *remote,
				      pjmedia_sdp_neg **p_neg)
{
    pjmedia_sdp_neg *neg;
    pj_status_t status;

    /* Check arguments are valid. */
    PJ_ASSERT_RETURN(pool && remote && p_neg, PJ_EINVAL);

    *p_neg = NULL;

    /* Validate remote offer and initial answer */
    status = pjmedia_sdp_validate2(remote, PJ_FALSE);
    if (status != PJ_SUCCESS)
	return status;

    /* Create and initialize negotiator. */
    neg = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_neg);
    PJ_ASSERT_RETURN(neg != NULL, PJ_ENOMEM);

    neg->prefer_remote_codec_order = PJMEDIA_SDP_NEG_PREFER_REMOTE_CODEC_ORDER;
    neg->answer_with_multiple_codecs = PJMEDIA_SDP_NEG_ANSWER_MULTIPLE_CODECS;
    neg->neg_remote_sdp = pjmedia_sdp_session_clone(pool, remote);

    if (initial) {
	PJ_ASSERT_RETURN((status=pjmedia_sdp_validate(initial))==PJ_SUCCESS, 
			 status);

	neg->initial_sdp = pjmedia_sdp_session_clone(pool, initial);
	neg->neg_local_sdp = pjmedia_sdp_session_clone(pool, initial);

	neg->state = PJMEDIA_SDP_NEG_STATE_WAIT_NEGO;

    } else {
	
	neg->state = PJMEDIA_SDP_NEG_STATE_REMOTE_OFFER;

    }

    *p_neg = neg;
    return PJ_SUCCESS;
}