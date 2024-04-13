static pj_status_t decode_binary_attr(pj_pool_t *pool, 
				      const pj_uint8_t *buf,
				      const pj_stun_msg_hdr *msghdr,
				      void **p_attr)
{
    pj_stun_binary_attr *attr;

    PJ_UNUSED_ARG(msghdr);

    /* Create the attribute */
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_binary_attr);
    GETATTRHDR(buf, &attr->hdr);

    /* Copy the data to the attribute */
    attr->length = attr->hdr.length;
    attr->data = (pj_uint8_t*) pj_pool_alloc(pool, attr->length);
    pj_memcpy(attr->data, buf+ATTR_HDR_LEN, attr->length);

    /* Done */
    *p_attr = attr;

    return PJ_SUCCESS;

}