PJ_DEF(pj_bool_t) pjmedia_sdp_neg_was_answer_remote(pjmedia_sdp_neg *neg)
{
    PJ_ASSERT_RETURN(neg, PJ_FALSE);

    return neg->answer_was_remote;
}