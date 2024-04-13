crypt_pw_md5_enc(const char *pwd)
{
    return crypt_pw_enc_by_hash(pwd, CRYPT_MD5);
}