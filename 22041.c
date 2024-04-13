PJ_DEF(const char*) pj_stun_get_method_name(unsigned msg_type)
{
    unsigned method = PJ_STUN_GET_METHOD(msg_type);

    if (method >= PJ_ARRAY_SIZE(stun_method_names))
	return "???";

    return stun_method_names[method];
}