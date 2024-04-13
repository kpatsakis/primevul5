static pj_status_t decode_unknown_attr(pj_pool_t *pool, 
				       const pj_uint8_t *buf, 
				       const pj_stun_msg_hdr *msghdr, 
				       void **p_attr)
{
    pj_stun_unknown_attr *attr;
    const pj_uint16_t *punk_attr;
    unsigned i;

    PJ_UNUSED_ARG(msghdr);

    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_unknown_attr);
    GETATTRHDR(buf, &attr->hdr);
 
    attr->attr_count = (attr->hdr.length >> 1);
    if (attr->attr_count > PJ_STUN_MAX_ATTR)
	return PJ_ETOOMANY;

    punk_attr = (const pj_uint16_t*)(buf + ATTR_HDR_LEN);
    for (i=0; i<attr->attr_count; ++i) {
	attr->attrs[i] = pj_ntohs(punk_attr[i]);
    }

    /* Done */
    *p_attr = attr;

    return PJ_SUCCESS;
}