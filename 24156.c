static int set_cn_and_email(X509 *crt, const char *name)
{
    return set_cn(crt, NID_commonName, name,
                  NID_pkcs9_emailAddress, "dummy@example.com", 0);
}