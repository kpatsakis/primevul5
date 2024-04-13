static int set_email1(X509 *crt, const char *name)
{
    return set_cn(crt, NID_pkcs9_emailAddress, name, 0);
}