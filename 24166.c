static int run_cert(X509 *crt, const char *nameincert,
                     const struct set_name_fn *fn)
{
    const char *const *pname = names;
    int failed = 0;

    for (; *pname != NULL; ++pname) {
        int samename = strcasecmp(nameincert, *pname) == 0;
        size_t namelen = strlen(*pname);
        char *name = OPENSSL_malloc(namelen + 1);
        int match, ret;

        memcpy(name, *pname, namelen + 1);

        match = -1;
        if (!TEST_int_ge(ret = X509_check_host(crt, name, namelen, 0, NULL),
                         0)) {
            failed = 1;
        } else if (fn->host) {
            if (ret == 1 && !samename)
                match = 1;
            if (ret == 0 && samename)
                match = 0;
        } else if (ret == 1)
            match = 1;
        if (!TEST_true(check_message(fn, "host", nameincert, match, *pname)))
            failed = 1;

        match = -1;
        if (!TEST_int_ge(ret = X509_check_host(crt, name, namelen,
                                               X509_CHECK_FLAG_NO_WILDCARDS,
                                               NULL), 0)) {
            failed = 1;
        } else if (fn->host) {
            if (ret == 1 && !samename)
                match = 1;
            if (ret == 0 && samename)
                match = 0;
        } else if (ret == 1)
            match = 1;
        if (!TEST_true(check_message(fn, "host-no-wildcards",
                                     nameincert, match, *pname)))
            failed = 1;

        match = -1;
        ret = X509_check_email(crt, name, namelen, 0);
        if (fn->email) {
            if (ret && !samename)
                match = 1;
            if (!ret && samename && strchr(nameincert, '@') != NULL)
                match = 0;
        } else if (ret)
            match = 1;
        if (!TEST_true(check_message(fn, "email", nameincert, match, *pname)))
            failed = 1;
        OPENSSL_free(name);
    }

    return failed == 0;
}