int ntlm_read_ntlm_v2_response(wStream* s, NTLMv2_RESPONSE* response)
{
	if (Stream_GetRemainingLength(s) < 16)
		return -1;
	Stream_Read(s, response->Response, 16);
	return ntlm_read_ntlm_v2_client_challenge(s, &(response->Challenge));
}