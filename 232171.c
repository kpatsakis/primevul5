crypt_pw_sha256_enc(const char *pwd)
{
    return crypt_pw_enc_by_hash(pwd, CRYPT_SHA256);
}