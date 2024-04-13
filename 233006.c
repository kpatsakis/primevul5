static int crypto_rsa_public(const BYTE* input, int length, UINT32 key_length, const BYTE* modulus,
                             const BYTE* exponent, BYTE* output)
{
	return crypto_rsa_common(input, length, key_length, modulus, exponent, EXPONENT_MAX_SIZE,
	                         output);
}