PJ_DEF(pjmedia_sdp_neg_state) pjmedia_sdp_neg_get_state( pjmedia_sdp_neg *neg )
{
    /* Check arguments are valid. */
    PJ_ASSERT_RETURN(neg != NULL, PJMEDIA_SDP_NEG_STATE_NULL);
    return neg->state;
}