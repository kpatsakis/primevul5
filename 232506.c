selftest_sign (gcry_sexp_t pkey, gcry_sexp_t skey)
{
  /* Sample data from RFC 6979 section A.2.5, hash is of message "sample" */
  static const char sample_data[] =
    "(data (flags rfc6979)"
    " (hash sha256 #af2bdbe1aa9b6ec1e2ade1d694f41fc71a831d0268e98915"
    /**/           "62113d8a62add1bf#))";
  static const char sample_data_bad[] =
    "(data (flags rfc6979)"
    " (hash sha256 #bf2bdbe1aa9b6ec1e2ade1d694f41fc71a831d0268e98915"
    /**/           "62113d8a62add1bf#))";
  static const char signature_r[] =
    "efd48b2aacb6a8fd1140dd9cd45e81d69d2c877b56aaf991c34d0ea84eaf3716";
  static const char signature_s[] =
    "f7cb1c942d657c41d436c7a1b6e29f65f3e900dbb9aff4064dc4ab2f843acda8";

  const char *errtxt = NULL;
  gcry_error_t err;
  gcry_sexp_t data = NULL;
  gcry_sexp_t data_bad = NULL;
  gcry_sexp_t sig = NULL;
  gcry_sexp_t l1 = NULL;
  gcry_sexp_t l2 = NULL;
  gcry_mpi_t r = NULL;
  gcry_mpi_t s = NULL;
  gcry_mpi_t calculated_r = NULL;
  gcry_mpi_t calculated_s = NULL;
  int cmp;

  err = sexp_sscan (&data, NULL, sample_data, strlen (sample_data));
  if (!err)
    err = sexp_sscan (&data_bad, NULL,
                      sample_data_bad, strlen (sample_data_bad));
  if (!err)
    err = _gcry_mpi_scan (&r, GCRYMPI_FMT_HEX, signature_r, 0, NULL);
  if (!err)
    err = _gcry_mpi_scan (&s, GCRYMPI_FMT_HEX, signature_s, 0, NULL);

  if (err)
    {
      errtxt = "converting data failed";
      goto leave;
    }

  err = _gcry_pk_sign (&sig, data, skey);
  if (err)
    {
      errtxt = "signing failed";
      goto leave;
    }

  /* check against known signature */
  errtxt = "signature validity failed";
  l1 = _gcry_sexp_find_token (sig, "sig-val", 0);
  if (!l1)
    goto leave;
  l2 = _gcry_sexp_find_token (l1, "ecdsa", 0);
  if (!l2)
    goto leave;

  sexp_release (l1);
  l1 = l2;

  l2 = _gcry_sexp_find_token (l1, "r", 0);
  if (!l2)
    goto leave;
  calculated_r = _gcry_sexp_nth_mpi (l2, 1, GCRYMPI_FMT_USG);
  if (!calculated_r)
    goto leave;

  sexp_release (l2);
  l2 = _gcry_sexp_find_token (l1, "s", 0);
  if (!l2)
    goto leave;
  calculated_s = _gcry_sexp_nth_mpi (l2, 1, GCRYMPI_FMT_USG);
  if (!calculated_s)
    goto leave;

  errtxt = "known sig check failed";

  cmp = _gcry_mpi_cmp (r, calculated_r);
  if (cmp)
    goto leave;
  cmp = _gcry_mpi_cmp (s, calculated_s);
  if (cmp)
    goto leave;

  errtxt = NULL;

  /* verify generated signature */
  err = _gcry_pk_verify (sig, data, pkey);
  if (err)
    {
      errtxt = "verify failed";
      goto leave;
    }
  err = _gcry_pk_verify (sig, data_bad, pkey);
  if (gcry_err_code (err) != GPG_ERR_BAD_SIGNATURE)
    {
      errtxt = "bad signature not detected";
      goto leave;
    }


 leave:
  sexp_release (sig);
  sexp_release (data_bad);
  sexp_release (data);
  sexp_release (l1);
  sexp_release (l2);
  mpi_release (r);
  mpi_release (s);
  mpi_release (calculated_r);
  mpi_release (calculated_s);
  return errtxt;
}