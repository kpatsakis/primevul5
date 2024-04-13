PJ_DEF(pj_status_t) pjmedia_sdp_neg_set_prefer_remote_codec_order(
						pjmedia_sdp_neg *neg,
						pj_bool_t prefer_remote)
{
    PJ_ASSERT_RETURN(neg, PJ_EINVAL);
    neg->prefer_remote_codec_order = prefer_remote;
    return PJ_SUCCESS;
}