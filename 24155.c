static int set_altname_email(X509 *crt, const char *name)
{
    return set_altname(crt, GEN_EMAIL, name, 0);
}