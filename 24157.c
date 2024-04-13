static int test_GENERAL_NAME_cmp(void)
{
    size_t i, j;
    GENERAL_NAME **namesa = OPENSSL_malloc(sizeof(*namesa)
                                           * OSSL_NELEM(gennames));
    GENERAL_NAME **namesb = OPENSSL_malloc(sizeof(*namesb)
                                           * OSSL_NELEM(gennames));
    int testresult = 0;

    if (!TEST_ptr(namesa) || !TEST_ptr(namesb))
        goto end;

    for (i = 0; i < OSSL_NELEM(gennames); i++) {
        const unsigned char *derp = gennames[i].der;

        /*
         * We create two versions of each GENERAL_NAME so that we ensure when
         * we compare them they are always different pointers.
         */
        namesa[i] = d2i_GENERAL_NAME(NULL, &derp, gennames[i].derlen);
        derp = gennames[i].der;
        namesb[i] = d2i_GENERAL_NAME(NULL, &derp, gennames[i].derlen);
        if (!TEST_ptr(namesa[i]) || !TEST_ptr(namesb[i]))
            goto end;
    }

    /* Every name should be equal to itself and not equal to any others. */
    for (i = 0; i < OSSL_NELEM(gennames); i++) {
        for (j = 0; j < OSSL_NELEM(gennames); j++) {
            if (i == j) {
                if (!TEST_int_eq(GENERAL_NAME_cmp(namesa[i], namesb[j]), 0))
                    goto end;
            } else {
                if (!TEST_int_ne(GENERAL_NAME_cmp(namesa[i], namesb[j]), 0))
                    goto end;
            }
        }
    }
    testresult = 1;

 end:
    for (i = 0; i < OSSL_NELEM(gennames); i++) {
        if (namesa != NULL)
            GENERAL_NAME_free(namesa[i]);
        if (namesb != NULL)
            GENERAL_NAME_free(namesb[i]);
    }
    OPENSSL_free(namesa);
    OPENSSL_free(namesb);

    return testresult;
}