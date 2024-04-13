static pj_status_t encode_binary_attr(const void *a, pj_uint8_t *buf, 
				      unsigned len, 
				      const pj_stun_msg_hdr *msghdr,
				      unsigned *printed)
{
    const pj_stun_binary_attr *ca = (const pj_stun_binary_attr*)a;

    PJ_CHECK_STACK();
    
    PJ_UNUSED_ARG(msghdr);

    /* Calculated total attr_len (add padding if necessary) */
    *printed = (ca->length + ATTR_HDR_LEN + 3) & (~3);
    if (len < *printed)
	return PJ_ETOOSMALL;

    PUTVAL16H(buf, 0, ca->hdr.type);
    PUTVAL16H(buf, 2, (pj_uint16_t) ca->length);

    /* Copy the data */
    pj_memcpy(buf+ATTR_HDR_LEN, ca->data, ca->length);

    /* Done */
    return PJ_SUCCESS;
}