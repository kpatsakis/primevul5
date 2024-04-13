g_get_tag_and_length(unsigned char **buf, int tag,
		     unsigned int buflen, unsigned int *outlen)
{
	unsigned char *ptr = *buf;
	int ret = -1; /* pessimists, assume failure ! */
	unsigned int encoded_len;
	int tmplen = 0;

	*outlen = 0;
	if (buflen > 1 && *ptr == tag) {
		ptr++;
		tmplen = gssint_get_der_length(&ptr, buflen - 1,
						&encoded_len);
		if (tmplen < 0) {
			ret = -1;
		} else if ((unsigned int)tmplen > buflen - (ptr - *buf)) {
			ret = -1;
		} else
			ret = 0;
	}
	*outlen = tmplen;
	*buf = ptr;
	return (ret);
}