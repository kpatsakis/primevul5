static int set_cn2(X509 *crt, const char *name)
{
    return set_cn(crt, NID_commonName, "dummy value",
                  NID_commonName, name, 0);
}