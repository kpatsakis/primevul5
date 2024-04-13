selftests_ecdsa (selftest_report_func_t report)
{
  const char *what;
  const char *errtxt;
  gcry_error_t err;
  gcry_sexp_t skey = NULL;
  gcry_sexp_t pkey = NULL;

  what = "convert";
  err = sexp_sscan (&skey, NULL, sample_secret_key_secp256,
                    strlen (sample_secret_key_secp256));
  if (!err)
    err = sexp_sscan (&pkey, NULL, sample_public_key_secp256,
                      strlen (sample_public_key_secp256));
  if (err)
    {
      errtxt = _gcry_strerror (err);
      goto failed;
    }

  what = "key consistency";
  err = ecc_check_secret_key(skey);
  if (err)
    {
      errtxt = _gcry_strerror (err);
      goto failed;
    }

  what = "sign";
  errtxt = selftest_sign (pkey, skey);
  if (errtxt)
    goto failed;

  sexp_release(pkey);
  sexp_release(skey);
  return 0; /* Succeeded. */

 failed:
  sexp_release(pkey);
  sexp_release(skey);
  if (report)
    report ("pubkey", GCRY_PK_ECC, what, errtxt);
  return GPG_ERR_SELFTEST_FAILED;
}