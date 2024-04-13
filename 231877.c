PJ_DEF(const char*) pjmedia_sdp_neg_state_str(pjmedia_sdp_neg_state state)
{
    if ((int)state >=0 && state < (pjmedia_sdp_neg_state)PJ_ARRAY_SIZE(state_str))
	return state_str[state];

    return "<?UNKNOWN?>";
}