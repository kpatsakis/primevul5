put_mech_set(gss_OID_set mechSet, gss_buffer_t buf)
{
	unsigned char *ptr;
	unsigned int i;
	unsigned int tlen, ilen;

	tlen = ilen = 0;
	for (i = 0; i < mechSet->count; i++) {
		/*
		 * 0x06 [DER LEN] [OID]
		 */
		ilen += 1 +
			gssint_der_length_size(mechSet->elements[i].length) +
			mechSet->elements[i].length;
	}
	/*
	 * 0x30 [DER LEN]
	 */
	tlen = 1 + gssint_der_length_size(ilen) + ilen;
	ptr = gssalloc_malloc(tlen);
	if (ptr == NULL)
		return -1;

	buf->value = ptr;
	buf->length = tlen;
#define REMAIN (buf->length - ((unsigned char *)buf->value - ptr))

	*ptr++ = SEQUENCE_OF;
	if (gssint_put_der_length(ilen, &ptr, REMAIN) < 0)
		return -1;
	for (i = 0; i < mechSet->count; i++) {
		if (put_mech_oid(&ptr, &mechSet->elements[i], REMAIN) < 0) {
			return -1;
		}
	}
	return 0;
#undef REMAIN
}