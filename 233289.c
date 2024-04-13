xdr_decode_string_inplace(__be32 *p, char **sp,
			  unsigned int *lenp, unsigned int maxlen)
{
	u32 len;

	len = be32_to_cpu(*p++);
	if (len > maxlen)
		return NULL;
	*lenp = len;
	*sp = (char *) p;
	return p + XDR_QUADLEN(len);
}