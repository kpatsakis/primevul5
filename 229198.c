int ntlm_write_ntlm_v2_response(wStream* s, NTLMv2_RESPONSE* response)
{
	Stream_Write(s, response->Response, 16);
	return ntlm_write_ntlm_v2_client_challenge(s, &(response->Challenge));
}