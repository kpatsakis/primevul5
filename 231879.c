PJ_DEF(pj_status_t) pjmedia_sdp_neg_cancel_offer(pjmedia_sdp_neg *neg)
{
    PJ_ASSERT_RETURN(neg, PJ_EINVAL);

    /* Must be in LOCAL_OFFER state. */
    PJ_ASSERT_RETURN(neg->state == PJMEDIA_SDP_NEG_STATE_LOCAL_OFFER ||
		     neg->state == PJMEDIA_SDP_NEG_STATE_REMOTE_OFFER,
		     PJMEDIA_SDPNEG_EINSTATE);

    if (neg->state == PJMEDIA_SDP_NEG_STATE_LOCAL_OFFER &&
	neg->active_local_sdp) 
    {
	/* Increment next version number. This happens if for example
	 * the reinvite offer is rejected by 488. If we don't increment
	 * the version here, the next offer will have the same version.
	 */
	neg->active_local_sdp->origin.version++;
    }

    /* Revert back initial SDP */
    if (neg->state == PJMEDIA_SDP_NEG_STATE_LOCAL_OFFER)
	neg->initial_sdp = neg->initial_sdp_tmp;

    /* Clear temporary SDP */
    neg->initial_sdp_tmp = NULL;
    neg->neg_local_sdp = neg->neg_remote_sdp = NULL;
    neg->has_remote_answer = PJ_FALSE;

    /* Reset state to done */
    neg->state = PJMEDIA_SDP_NEG_STATE_DONE;

    return PJ_SUCCESS;
}