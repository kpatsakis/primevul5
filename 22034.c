static pj_status_t encode_msgint_attr(const void *a, pj_uint8_t *buf, 
				      unsigned len, 
				      const pj_stun_msg_hdr *msghdr,
				      unsigned *printed)
{
    const pj_stun_msgint_attr *ca = (const pj_stun_msgint_attr*)a;

    PJ_CHECK_STACK();
    
    PJ_UNUSED_ARG(msghdr);

    if (len < 24) 
	return PJ_ETOOSMALL;

    /* Copy and convert attribute to network byte order */
    PUTVAL16H(buf, 0, ca->hdr.type);
    PUTVAL16H(buf, 2, ca->hdr.length);

    pj_memcpy(buf+4, ca->hmac, 20);

    /* Done */
    *printed = 24;

    return PJ_SUCCESS;
}