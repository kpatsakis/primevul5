static int set_email3(X509 *crt, const char *name)
{
    return set_cn(crt, NID_pkcs9_emailAddress, name,
                  NID_pkcs9_emailAddress, "dummy@example.com", 0);
}