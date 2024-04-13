ecc_check_secret_key (gcry_sexp_t keyparms)
{
  gcry_err_code_t rc;
  gcry_sexp_t l1 = NULL;
  int flags = 0;
  char *curvename = NULL;
  gcry_mpi_t mpi_g = NULL;
  gcry_mpi_t mpi_q = NULL;
  ECC_secret_key sk;
  mpi_ec_t ec = NULL;

  memset (&sk, 0, sizeof sk);

  /* Look for flags. */
  l1 = sexp_find_token (keyparms, "flags", 0);
  if (l1)
    {
      rc = _gcry_pk_util_parse_flaglist (l1, &flags, NULL);
      if (rc)
        goto leave;
    }

  /* Extract the parameters.  */
  if ((flags & PUBKEY_FLAG_PARAM))
    rc = sexp_extract_param (keyparms, NULL, "-p?a?b?g?n?h?/q?+d",
                             &sk.E.p, &sk.E.a, &sk.E.b, &mpi_g, &sk.E.n,
                             &sk.E.h, &mpi_q, &sk.d, NULL);
  else
    rc = sexp_extract_param (keyparms, NULL, "/q?+d",
                             &mpi_q, &sk.d, NULL);
  if (rc)
    goto leave;

  /* Add missing parameters using the optional curve parameter.  */
  sexp_release (l1);
  l1 = sexp_find_token (keyparms, "curve", 5);
  if (l1)
    {
      curvename = sexp_nth_string (l1, 1);
      if (curvename)
        {
          rc = _gcry_ecc_fill_in_curve (0, curvename, &sk.E, NULL);
          if (rc)
            goto leave;
        }
    }
  if (mpi_g)
    {
      if (!sk.E.G.x)
        point_init (&sk.E.G);
      rc = _gcry_ecc_os2ec (&sk.E.G, mpi_g);
      if (rc)
        goto leave;
    }

  /* Guess required fields if a curve parameter has not been given.
     FIXME: This is a crude hacks.  We need to fix that.  */
  if (!curvename)
    {
      sk.E.model = ((flags & PUBKEY_FLAG_EDDSA)
               ? MPI_EC_EDWARDS
               : MPI_EC_WEIERSTRASS);
      sk.E.dialect = ((flags & PUBKEY_FLAG_EDDSA)
                      ? ECC_DIALECT_ED25519
                      : ECC_DIALECT_STANDARD);
      if (!sk.E.h)
	sk.E.h = mpi_const (MPI_C_ONE);
    }
  if (DBG_CIPHER)
    {
      log_debug ("ecc_testkey inf: %s/%s\n",
                 _gcry_ecc_model2str (sk.E.model),
                 _gcry_ecc_dialect2str (sk.E.dialect));
      if (sk.E.name)
        log_debug  ("ecc_testkey nam: %s\n", sk.E.name);
      log_printmpi ("ecc_testkey   p", sk.E.p);
      log_printmpi ("ecc_testkey   a", sk.E.a);
      log_printmpi ("ecc_testkey   b", sk.E.b);
      log_printpnt ("ecc_testkey g",   &sk.E.G, NULL);
      log_printmpi ("ecc_testkey   n", sk.E.n);
      log_printmpi ("ecc_testkey   h", sk.E.h);
      log_printmpi ("ecc_testkey   q", mpi_q);
      if (!fips_mode ())
        log_printmpi ("ecc_testkey   d", sk.d);
    }
  if (!sk.E.p || !sk.E.a || !sk.E.b || !sk.E.G.x || !sk.E.n || !sk.E.h || !sk.d)
    {
      rc = GPG_ERR_NO_OBJ;
      goto leave;
    }

  ec = _gcry_mpi_ec_p_internal_new (sk.E.model, sk.E.dialect, flags,
                                    sk.E.p, sk.E.a, sk.E.b);

  if (mpi_q)
    {
      point_init (&sk.Q);
      if (ec->dialect == ECC_DIALECT_ED25519)
        rc = _gcry_ecc_eddsa_decodepoint (mpi_q, ec, &sk.Q, NULL, NULL);
      else if (ec->model == MPI_EC_MONTGOMERY)
        rc = _gcry_ecc_mont_decodepoint (mpi_q, ec, &sk.Q);
      else
        rc = _gcry_ecc_os2ec (&sk.Q, mpi_q);
      if (rc)
        goto leave;
    }
  else
    {
      /* The secret key test requires Q.  */
      rc = GPG_ERR_NO_OBJ;
      goto leave;
    }

  if (check_secret_key (&sk, ec, flags))
    rc = GPG_ERR_BAD_SECKEY;

 leave:
  _gcry_mpi_ec_free (ec);
  _gcry_mpi_release (sk.E.p);
  _gcry_mpi_release (sk.E.a);
  _gcry_mpi_release (sk.E.b);
  _gcry_mpi_release (mpi_g);
  point_free (&sk.E.G);
  _gcry_mpi_release (sk.E.n);
  _gcry_mpi_release (sk.E.h);
  _gcry_mpi_release (mpi_q);
  point_free (&sk.Q);
  _gcry_mpi_release (sk.d);
  xfree (curvename);
  sexp_release (l1);
  if (DBG_CIPHER)
    log_debug ("ecc_testkey   => %s\n", gpg_strerror (rc));
  return rc;
}