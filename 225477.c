make_spnego_tokenTarg_msg(OM_uint32 status, gss_OID mech_wanted,
			  gss_buffer_t data, gss_buffer_t mechListMIC,
			  send_token_flag sendtoken,
			  gss_buffer_t outbuf)
{
	unsigned int tlen = 0;
	unsigned int ret = 0;
	unsigned int NegTokenTargSize = 0;
	unsigned int NegTokenSize = 0;
	unsigned int rspTokenSize = 0;
	unsigned int micTokenSize = 0;
	unsigned int dataLen = 0;
	unsigned char *t;
	unsigned char *ptr;

	if (outbuf == GSS_C_NO_BUFFER)
		return (GSS_S_DEFECTIVE_TOKEN);
	if (sendtoken == INIT_TOKEN_SEND && mech_wanted == GSS_C_NO_OID)
	    return (GSS_S_DEFECTIVE_TOKEN);

	outbuf->length = 0;
	outbuf->value = NULL;

	/*
	 * ASN.1 encoding of the negResult
	 * ENUMERATED type is 3 bytes
	 *  ENUMERATED TAG, Length, Value,
	 * Plus 2 bytes for the CONTEXT id and length.
	 */
	dataLen = 5;

	/*
	 * calculate data length
	 *
	 * If this is the initial token, include length of
	 * mech_type and the negotiation result fields.
	 */
	if (sendtoken == INIT_TOKEN_SEND) {
		int mechlistTokenSize;
		/*
		 * 1 byte for the CONTEXT ID(0xa0),
		 * 1 byte for the OID ID(0x06)
		 * 1 byte for OID Length field
		 * Plus the rest... (OID Length, OID value)
		 */
		mechlistTokenSize = 3 + mech_wanted->length +
			gssint_der_length_size(mech_wanted->length);

		dataLen += mechlistTokenSize;
	}
	if (data != NULL && data->length > 0) {
		/* Length of the inner token */
		rspTokenSize = 1 + gssint_der_length_size(data->length) +
			data->length;

		dataLen += rspTokenSize;

		/* Length of the outer token */
		dataLen += 1 + gssint_der_length_size(rspTokenSize);
	}
	if (mechListMIC != NULL) {

		/* Length of the inner token */
		micTokenSize = 1 + gssint_der_length_size(mechListMIC->length) +
			mechListMIC->length;

		dataLen += micTokenSize;

		/* Length of the outer token */
		dataLen += 1 + gssint_der_length_size(micTokenSize);
	}
	/*
	 * Add size of DER encoded:
	 * NegTokenTarg [ SEQUENCE ] of
	 *    NegResult[0] ENUMERATED {
	 *	accept_completed(0),
	 *	accept_incomplete(1),
	 *	reject(2) }
	 *    supportedMech [1] MechType OPTIONAL,
	 *    responseToken [2] OCTET STRING OPTIONAL,
	 *    mechListMIC   [3] OCTET STRING OPTIONAL
	 *
	 * size = data->length + MechListMic + SupportedMech len +
	 *	Result Length + ASN.1 overhead
	 */
	NegTokenTargSize = dataLen;
	dataLen += 1 + gssint_der_length_size(NegTokenTargSize);

	/*
	 * NegotiationToken [ CHOICE ]{
	 *    negTokenInit  [0]	 NegTokenInit,
	 *    negTokenTarg  [1]	 NegTokenTarg }
	 */
	NegTokenSize = dataLen;
	dataLen += 1 + gssint_der_length_size(NegTokenSize);

	tlen = dataLen;
	t = (unsigned char *) gssalloc_malloc(tlen);

	if (t == NULL) {
		ret = GSS_S_DEFECTIVE_TOKEN;
		goto errout;
	}

	ptr = t;

	/*
	 * Indicate that we are sending CHOICE 1
	 * (NegTokenTarg)
	 */
	*ptr++ = CONTEXT | 0x01;
	if (gssint_put_der_length(NegTokenSize, &ptr, dataLen) < 0) {
		ret = GSS_S_DEFECTIVE_TOKEN;
		goto errout;
	}
	*ptr++ = SEQUENCE;
	if (gssint_put_der_length(NegTokenTargSize, &ptr,
				  tlen - (int)(ptr-t)) < 0) {
		ret = GSS_S_DEFECTIVE_TOKEN;
		goto errout;
	}

	/*
	 * First field of the NegTokenTarg SEQUENCE
	 * is the ENUMERATED NegResult.
	 */
	*ptr++ = CONTEXT;
	if (gssint_put_der_length(3, &ptr,
				  tlen - (int)(ptr-t)) < 0) {
		ret = GSS_S_DEFECTIVE_TOKEN;
		goto errout;
	}
	if (put_negResult(&ptr, status, tlen - (int)(ptr - t)) < 0) {
		ret = GSS_S_DEFECTIVE_TOKEN;
		goto errout;
	}
	if (sendtoken == INIT_TOKEN_SEND) {
		/*
		 * Next, is the Supported MechType
		 */
		*ptr++ = CONTEXT | 0x01;
		if (gssint_put_der_length(mech_wanted->length + 2,
					  &ptr,
					  tlen - (int)(ptr - t)) < 0) {
			ret = GSS_S_DEFECTIVE_TOKEN;
			goto errout;
		}
		if (put_mech_oid(&ptr, mech_wanted,
				 tlen - (int)(ptr - t)) < 0) {
			ret = GSS_S_DEFECTIVE_TOKEN;
			goto errout;
		}
	}
	if (data != NULL && data->length > 0) {
		*ptr++ = CONTEXT | 0x02;
		if (gssint_put_der_length(rspTokenSize, &ptr,
					  tlen - (int)(ptr - t)) < 0) {
			ret = GSS_S_DEFECTIVE_TOKEN;
			goto errout;
		}
		if (put_input_token(&ptr, data,
				    tlen - (int)(ptr - t)) < 0) {
			ret = GSS_S_DEFECTIVE_TOKEN;
			goto errout;
		}
	}
	if (mechListMIC != NULL) {
		*ptr++ = CONTEXT | 0x03;
		if (gssint_put_der_length(micTokenSize, &ptr,
					  tlen - (int)(ptr - t)) < 0) {
			ret = GSS_S_DEFECTIVE_TOKEN;
			goto errout;
		}
		if (put_input_token(&ptr, mechListMIC,
				    tlen - (int)(ptr - t)) < 0) {
			ret = GSS_S_DEFECTIVE_TOKEN;
			goto errout;
		}
	}
	ret = GSS_S_COMPLETE;
errout:
	if (ret != GSS_S_COMPLETE) {
		if (t)
			free(t);
	} else {
		outbuf->length = ptr - t;
		outbuf->value = (void *) t;
	}

	return (ret);
}