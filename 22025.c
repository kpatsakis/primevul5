PJ_DEF(const char*) pj_stun_get_class_name(unsigned msg_type)
{
    if (PJ_STUN_IS_REQUEST(msg_type))
	return "request";
    else if (PJ_STUN_IS_SUCCESS_RESPONSE(msg_type))
	return "success response";
    else if (PJ_STUN_IS_ERROR_RESPONSE(msg_type))
	return "error response";
    else if (PJ_STUN_IS_INDICATION(msg_type))
	return "indication";
    else
	return "???";
}