void ntlm_rc4k(BYTE* key, int length, BYTE* plaintext, BYTE* ciphertext)
{
	WINPR_RC4_CTX* rc4 = winpr_RC4_New(key, 16);

	if (rc4)
	{
		winpr_RC4_Update(rc4, length, plaintext, ciphertext);
		winpr_RC4_Free(rc4);
	}
}