PJ_DEF(pj_status_t) pj_stun_msg_check(const pj_uint8_t *pdu, pj_size_t pdu_len,
				      unsigned options)
{
    pj_uint32_t msg_len;

    PJ_ASSERT_RETURN(pdu, PJ_EINVAL);

    if (pdu_len < sizeof(pj_stun_msg_hdr))
	return PJNATH_EINSTUNMSGLEN;

    /* First byte of STUN message is always 0x00 or 0x01. */
    if (*pdu != 0x00 && *pdu != 0x01)
	return PJNATH_EINSTUNMSGTYPE;

    /* Check the PDU length */
    msg_len = GETVAL16H(pdu, 2);
    if ((msg_len + 20 > pdu_len) || 
	((options & PJ_STUN_IS_DATAGRAM) && msg_len + 20 != pdu_len))
    {
	return PJNATH_EINSTUNMSGLEN;
    }

    /* STUN message is always padded to the nearest 4 bytes, thus
     * the last two bits of the length field are always zero.
     */
    if ((msg_len & 0x03) != 0) {
	return PJNATH_EINSTUNMSGLEN;
    }

    /* If magic is set, then there is great possibility that this is
     * a STUN message.
     */
    if (GETVAL32H(pdu, 4) == PJ_STUN_MAGIC) {

	/* Check if FINGERPRINT attribute is present */
	if ((options & PJ_STUN_NO_FINGERPRINT_CHECK )==0 && 
	    GETVAL16H(pdu, msg_len + 20 - 8) == PJ_STUN_ATTR_FINGERPRINT) 
	{
	    pj_uint16_t attr_len = GETVAL16H(pdu, msg_len + 20 - 8 + 2);
	    pj_uint32_t fingerprint = GETVAL32H(pdu, msg_len + 20 - 8 + 4);
	    pj_uint32_t crc;

	    if (attr_len != 4)
		return PJNATH_ESTUNINATTRLEN;

	    crc = pj_crc32_calc(pdu, msg_len + 20 - 8);
	    crc ^= STUN_XOR_FINGERPRINT;

	    if (crc != fingerprint)
		return PJNATH_ESTUNFINGERPRINT;
	}
    }

    /* Could be a STUN message */
    return PJ_SUCCESS;
}