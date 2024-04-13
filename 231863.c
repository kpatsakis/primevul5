PJ_DEF(pj_status_t) pjmedia_sdp_neg_set_remote_offer( pj_pool_t *pool,
				  pjmedia_sdp_neg *neg,
				  const pjmedia_sdp_session *remote)
{
    /* Check arguments are valid. */
    PJ_ASSERT_RETURN(pool && neg && remote, PJ_EINVAL);

    /* Can only do this in STATE_DONE.
     * If we already provide local offer, then rx_remote_answer() should
     * be called instead of this function.
     */
    PJ_ASSERT_RETURN(neg->state == PJMEDIA_SDP_NEG_STATE_DONE, 
		     PJMEDIA_SDPNEG_EINSTATE);

    /* State now is STATE_REMOTE_OFFER. */
    neg->state = PJMEDIA_SDP_NEG_STATE_REMOTE_OFFER;
    neg->neg_remote_sdp = pjmedia_sdp_session_clone(pool, remote);

    return PJ_SUCCESS;
}