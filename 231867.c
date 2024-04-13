PJ_DEF(pj_status_t) pjmedia_sdp_neg_set_remote_answer( pj_pool_t *pool,
				   pjmedia_sdp_neg *neg,
				   const pjmedia_sdp_session *remote)
{
    /* Check arguments are valid. */
    PJ_ASSERT_RETURN(pool && neg && remote, PJ_EINVAL);

    /* Can only do this in STATE_LOCAL_OFFER.
     * If we haven't provided local offer, then rx_remote_offer() should
     * be called instead of this function.
     */
    PJ_ASSERT_RETURN(neg->state == PJMEDIA_SDP_NEG_STATE_LOCAL_OFFER, 
		     PJMEDIA_SDPNEG_EINSTATE);

    /* We're ready to negotiate. */
    neg->state = PJMEDIA_SDP_NEG_STATE_WAIT_NEGO;
    neg->has_remote_answer = PJ_TRUE;
    neg->neg_remote_sdp = pjmedia_sdp_session_clone(pool, remote);
 
    return PJ_SUCCESS;
}