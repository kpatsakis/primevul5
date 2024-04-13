PJ_DEF(pj_status_t) pjmedia_sdp_neg_negotiate( pj_pool_t *pool,
					       pjmedia_sdp_neg *neg,
					       pj_bool_t allow_asym)
{
    pj_status_t status;

    /* Check arguments are valid. */
    PJ_ASSERT_RETURN(pool && neg, PJ_EINVAL);

    /* Must be in STATE_WAIT_NEGO state. */
    PJ_ASSERT_RETURN(neg->state == PJMEDIA_SDP_NEG_STATE_WAIT_NEGO, 
		     PJMEDIA_SDPNEG_EINSTATE);

    /* Must have remote offer. */
    PJ_ASSERT_RETURN(neg->neg_remote_sdp, PJ_EBUG);

    if (neg->has_remote_answer) {
	pjmedia_sdp_session *active;
	status = process_answer(pool, neg->neg_local_sdp, neg->neg_remote_sdp,
			        allow_asym, &active);
	if (status == PJ_SUCCESS) {
	    /* Only update active SDPs when negotiation is successfull */
	    neg->active_local_sdp = active;
	    neg->active_remote_sdp = neg->neg_remote_sdp;
	}
    } else {
	pjmedia_sdp_session *answer = NULL;

	status = create_answer(pool, neg->prefer_remote_codec_order,
                               neg->answer_with_multiple_codecs,
			       neg->neg_local_sdp, neg->neg_remote_sdp,
			       &answer);
	if (status == PJ_SUCCESS) {
	    pj_uint32_t active_ver;

	    if (neg->active_local_sdp)
		active_ver = neg->active_local_sdp->origin.version;
	    else
		active_ver = neg->initial_sdp->origin.version;

#if PJMEDIA_SDP_NEG_COMPARE_BEFORE_INC_VERSION
	    answer->origin.version = active_ver;

	    if ((neg->active_local_sdp == NULL) || 
		(pjmedia_sdp_session_cmp(answer, neg->active_local_sdp, 0) 
								!= PJ_SUCCESS))
	    {
		++answer->origin.version;
	    }
#else
	    answer->origin.version = active_ver + 1;
#endif	    
	    /* Only update active SDPs when negotiation is successfull */
	    neg->active_local_sdp = answer;
	    neg->active_remote_sdp = neg->neg_remote_sdp;
	}
    }

    /* State is DONE regardless */
    neg->state = PJMEDIA_SDP_NEG_STATE_DONE;

    /* Save state */
    neg->answer_was_remote = neg->has_remote_answer;

    /* Revert back initial SDP if nego fails */
    if (status != PJ_SUCCESS)
	neg->initial_sdp = neg->initial_sdp_tmp;

    /* Clear temporary SDP */
    neg->initial_sdp_tmp = NULL;
    neg->neg_local_sdp = neg->neg_remote_sdp = NULL;
    neg->has_remote_answer = PJ_FALSE;

    return status;
}