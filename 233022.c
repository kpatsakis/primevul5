int crypto_rsa_private_encrypt(const BYTE* input, int length, UINT32 key_length,
                               const BYTE* modulus, const BYTE* private_exponent, BYTE* output)
{
	return crypto_rsa_private(input, length, key_length, modulus, private_exponent, output);
}