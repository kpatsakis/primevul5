static pj_status_t decode_string_attr(pj_pool_t *pool, 
				      const pj_uint8_t *buf, 
				      const pj_stun_msg_hdr *msghdr, 
				      void **p_attr)
{
    pj_stun_string_attr *attr;
    pj_str_t value;

    PJ_UNUSED_ARG(msghdr);

    /* Create the attribute */
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_string_attr);
    GETATTRHDR(buf, &attr->hdr);

    /* Get pointer to the string in the message */
    value.ptr = ((char*)buf + ATTR_HDR_LEN);
    value.slen = attr->hdr.length;

    /* Copy the string to the attribute */
    pj_strdup(pool, &attr->value, &value);

    /* Done */
    *p_attr = attr;

    return PJ_SUCCESS;

}