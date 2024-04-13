static int set_cn(X509 *crt, ...)
{
    int ret = 0;
    X509_NAME *n = NULL;
    va_list ap;

    va_start(ap, crt);
    n = X509_NAME_new();
    if (n == NULL)
        goto out;

    while (1) {
        int nid;
        const char *name;

        nid = va_arg(ap, int);
        if (nid == 0)
            break;
        name = va_arg(ap, const char *);
        if (!X509_NAME_add_entry_by_NID(n, nid, MBSTRING_ASC,
                                        (unsigned char *)name, -1, -1, 1))
            goto out;
    }
    if (!X509_set_subject_name(crt, n))
        goto out;
    ret = 1;
 out:
    X509_NAME_free(n);
    va_end(ap);
    return ret;
}