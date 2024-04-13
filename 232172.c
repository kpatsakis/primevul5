crypt_pw_sha512_enc(const char *pwd)
{
    return crypt_pw_enc_by_hash(pwd, CRYPT_SHA512);
}