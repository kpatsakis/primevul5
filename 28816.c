static inline unsigned short dns_response_get_query_id(unsigned char *resp)
{
	return resp[0] * 256 + resp[1];
}
