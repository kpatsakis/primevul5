PJ_DEF(pj_status_t) pjmedia_sdp_neg_set_local_answer( pj_pool_t *pool,
				  pjmedia_sdp_neg *neg,
				  const pjmedia_sdp_session *local)
{
    /* Check arguments are valid. */
    PJ_ASSERT_RETURN(pool && neg && local, PJ_EINVAL);

    /* Can only do this in STATE_REMOTE_OFFER or WAIT_NEGO.
     * If we already provide local offer, then set_remote_answer() should
     * be called instead of this function.
     */
    PJ_ASSERT_RETURN(neg->state == PJMEDIA_SDP_NEG_STATE_REMOTE_OFFER ||
    		     neg->state == PJMEDIA_SDP_NEG_STATE_WAIT_NEGO, 
		     PJMEDIA_SDPNEG_EINSTATE);

    /* State now is STATE_WAIT_NEGO. */
    neg->state = PJMEDIA_SDP_NEG_STATE_WAIT_NEGO;
    if (local) {
	neg->neg_local_sdp = pjmedia_sdp_session_clone(pool, local);
	if (neg->initial_sdp) {
	    /* Retain initial_sdp value. */
	    neg->initial_sdp_tmp = neg->initial_sdp;
	    neg->initial_sdp = pjmedia_sdp_session_clone(pool,
							 neg->initial_sdp);
        
	    /* I don't think there is anything in RFC 3264 that mandates
	     * answerer to place the same origin (and increment version)
	     * in the answer, but probably it won't hurt either.
	     * Note that the version will be incremented in 
	     * pjmedia_sdp_neg_negotiate()
	     */
	    neg->neg_local_sdp->origin.id = neg->initial_sdp->origin.id;
	} else {
	    neg->initial_sdp = pjmedia_sdp_session_clone(pool, local);
	}
    } else {
	PJ_ASSERT_RETURN(neg->initial_sdp, PJMEDIA_SDPNEG_ENOINITIAL);
	neg->initial_sdp_tmp = neg->initial_sdp;
	neg->initial_sdp = pjmedia_sdp_session_clone(pool, neg->initial_sdp);
	neg->neg_local_sdp = pjmedia_sdp_session_clone(pool, neg->initial_sdp);
    }

    return PJ_SUCCESS;
}