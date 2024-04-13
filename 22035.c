PJ_DEF(pj_stun_attr_hdr*) pj_stun_msg_find_attr( const pj_stun_msg *msg,
						 int attr_type,
						 unsigned index)
{
    PJ_ASSERT_RETURN(msg, NULL);

    for (; index < msg->attr_count; ++index) {
	if (msg->attr[index]->type == attr_type)
	    return (pj_stun_attr_hdr*) msg->attr[index];
    }

    return NULL;
}