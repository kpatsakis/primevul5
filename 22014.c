static pj_status_t encode_unknown_attr(const void *a, pj_uint8_t *buf, 
				       unsigned len, 
				       const pj_stun_msg_hdr *msghdr,
				       unsigned *printed)
{
    const pj_stun_unknown_attr *ca = (const pj_stun_unknown_attr*) a;
    pj_uint16_t *dst_unk_attr;
    unsigned i;

    PJ_CHECK_STACK();
    
    PJ_UNUSED_ARG(msghdr);

    /* Check that buffer is enough */
    if (len < ATTR_HDR_LEN + (ca->attr_count << 1))
	return PJ_ETOOSMALL;

    PUTVAL16H(buf, 0, ca->hdr.type);
    PUTVAL16H(buf, 2, (pj_uint16_t)(ca->attr_count << 1));

    /* Copy individual attribute */
    dst_unk_attr = (pj_uint16_t*)(buf + ATTR_HDR_LEN);
    for (i=0; i < ca->attr_count; ++i, ++dst_unk_attr) {
	*dst_unk_attr = pj_htons(ca->attrs[i]);
    }

    /* Done */
    *printed = (ATTR_HDR_LEN + (ca->attr_count << 1) + 3) & (~3);

    return PJ_SUCCESS;
}