PJ_DEF(pj_status_t) pjmedia_sdp_neg_set_answer_multiple_codecs(
                        pjmedia_sdp_neg *neg,
                        pj_bool_t answer_multiple)
{
    PJ_ASSERT_RETURN(neg, PJ_EINVAL);
    neg->answer_with_multiple_codecs = answer_multiple;
    return PJ_SUCCESS;
}