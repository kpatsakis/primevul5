PJ_DEF(pj_status_t) pjmedia_sdp_neg_create_w_local_offer( pj_pool_t *pool,
				      const pjmedia_sdp_session *local,
				      pjmedia_sdp_neg **p_neg)
{
    pjmedia_sdp_neg *neg;
    pj_status_t status;

    /* Check arguments are valid. */
    PJ_ASSERT_RETURN(pool && local && p_neg, PJ_EINVAL);

    *p_neg = NULL;

    /* Validate local offer. */
    PJ_ASSERT_RETURN((status=pjmedia_sdp_validate(local))==PJ_SUCCESS, status);

    /* Create and initialize negotiator. */
    neg = PJ_POOL_ZALLOC_T(pool, pjmedia_sdp_neg);
    PJ_ASSERT_RETURN(neg != NULL, PJ_ENOMEM);

    neg->state = PJMEDIA_SDP_NEG_STATE_LOCAL_OFFER;
    neg->prefer_remote_codec_order = PJMEDIA_SDP_NEG_PREFER_REMOTE_CODEC_ORDER;
    neg->answer_with_multiple_codecs = PJMEDIA_SDP_NEG_ANSWER_MULTIPLE_CODECS;
    neg->initial_sdp = pjmedia_sdp_session_clone(pool, local);
    neg->neg_local_sdp = pjmedia_sdp_session_clone(pool, local);

    *p_neg = neg;
    return PJ_SUCCESS;
}