static int call_run_cert(int i)
{
    int failed = 0;
    const struct set_name_fn *pfn = &name_fns[i];
    X509 *crt;
    const char *const *pname;

    TEST_info("%s", pfn->name);
    for (pname = names; *pname != NULL; pname++) {
        if (!TEST_ptr(crt = make_cert())
             || !TEST_true(pfn->fn(crt, *pname))
             || !run_cert(crt, *pname, pfn))
            failed = 1;
        X509_free(crt);
    }
    return failed == 0;
}