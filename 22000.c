static pj_status_t decode_empty_attr(pj_pool_t *pool, 
				     const pj_uint8_t *buf, 
				     const pj_stun_msg_hdr *msghdr,
				     void **p_attr)
{
    pj_stun_empty_attr *attr;

    PJ_UNUSED_ARG(msghdr);

    /* Check that the struct address is valid */
    pj_assert(sizeof(pj_stun_empty_attr) == ATTR_HDR_LEN);

    /* Create the attribute */
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_empty_attr);
    GETATTRHDR(buf, &attr->hdr);

    /* Check that the attribute length is valid */
    if (attr->hdr.length != 0)
	return PJNATH_ESTUNINATTRLEN;

    /* Done */
    *p_attr = attr;

    return PJ_SUCCESS;
}