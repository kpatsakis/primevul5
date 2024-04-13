PJ_DEF(pj_status_t) pjmedia_sdp_neg_get_neg_remote( pjmedia_sdp_neg *neg,
				const pjmedia_sdp_session **remote)
{
    PJ_ASSERT_RETURN(neg && remote, PJ_EINVAL);
    PJ_ASSERT_RETURN(neg->neg_remote_sdp, PJMEDIA_SDPNEG_ENONEG);

    *remote = neg->neg_remote_sdp;
    return PJ_SUCCESS;
}