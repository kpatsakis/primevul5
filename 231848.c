PJ_DEF(pj_bool_t) pjmedia_sdp_neg_has_local_answer(pjmedia_sdp_neg *neg)
{
    pj_assert(neg && neg->state==PJMEDIA_SDP_NEG_STATE_WAIT_NEGO);
    return !neg->has_remote_answer;
}