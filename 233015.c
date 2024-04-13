static int crypto_rsa_decrypt(const BYTE* input, int length, UINT32 key_length, const BYTE* modulus,
                              const BYTE* private_exponent, BYTE* output)
{
	return crypto_rsa_common(input, length, key_length, modulus, private_exponent, key_length,
	                         output);
}