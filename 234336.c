static inline uint32_t ntlmssp_flags(uint32_t client_flags)
{
	uint32_t flags = NTLMSSP_NEGOTIATE_NTLM |
			 NTLMSSP_NEGOTIATE_TARGET_INFO;

	if ((client_flags & NTLMSSP_NEGOTIATE_UNICODE) != 0)
		flags |= NTLMSSP_NEGOTIATE_UNICODE;
	else
		flags |= NTLMSSP_NEGOTIATE_OEM;

	if ((client_flags & NTLMSSP_NEGOTIATE_NTLM2) != 0)
		flags |= NTLMSSP_NEGOTIATE_NTLM2;

	if ((client_flags & NTLMSSP_REQUEST_TARGET) != 0)
		flags |= NTLMSSP_REQUEST_TARGET | NTLMSSP_TARGET_TYPE_SERVER;

	return flags;
}