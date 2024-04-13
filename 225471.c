put_negResult(unsigned char **buf_out, OM_uint32 negResult,
	      unsigned int buflen)
{
	if (buflen < 3)
		return (-1);
	*(*buf_out)++ = ENUMERATED;
	*(*buf_out)++ = ENUMERATION_LENGTH;
	*(*buf_out)++ = (unsigned char) negResult;
	return (0);
}