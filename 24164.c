static int set_cn1(X509 *crt, const char *name)
{
    return set_cn(crt, NID_commonName, name, 0);
}