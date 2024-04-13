int crypto_rsa_public_encrypt(const BYTE* input, int length, UINT32 key_length, const BYTE* modulus,
                              const BYTE* exponent, BYTE* output)
{
	return crypto_rsa_public(input, length, key_length, modulus, exponent, output);
}