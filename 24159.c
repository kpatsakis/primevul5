static X509 *make_cert(void)
{
    X509 *crt = NULL;

    if (!TEST_ptr(crt = X509_new()))
        return NULL;
    if (!TEST_true(X509_set_version(crt, 2))) {
        X509_free(crt);
        return NULL;
    }
    return crt;
}