PJ_DEF(pj_status_t) pjmedia_sdp_neg_get_active_local( pjmedia_sdp_neg *neg,
					const pjmedia_sdp_session **local)
{
    PJ_ASSERT_RETURN(neg && local, PJ_EINVAL);
    PJ_ASSERT_RETURN(neg->active_local_sdp, PJMEDIA_SDPNEG_ENOACTIVE);

    *local = neg->active_local_sdp;
    return PJ_SUCCESS;
}