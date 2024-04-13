xdr_encode_string(__be32 *p, const char *string)
{
	return xdr_encode_array(p, string, strlen(string));
}