ecc_sign (gcry_sexp_t *r_sig, gcry_sexp_t s_data, gcry_sexp_t keyparms)
{
  gcry_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_mpi_t data = NULL;
  gcry_sexp_t l1 = NULL;
  char *curvename = NULL;
  gcry_mpi_t mpi_g = NULL;
  gcry_mpi_t mpi_q = NULL;
  ECC_secret_key sk;
  gcry_mpi_t sig_r = NULL;
  gcry_mpi_t sig_s = NULL;

  memset (&sk, 0, sizeof sk);

  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_SIGN, 0);

  /* Extract the data.  */
  rc = _gcry_pk_util_data_to_mpi (s_data, &data, &ctx);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_mpidump ("ecc_sign   data", data);

  /*
   * Extract the key.
   */
  if ((ctx.flags & PUBKEY_FLAG_PARAM))
    rc = sexp_extract_param (keyparms, NULL, "-p?a?b?g?n?h?/q?+d",
                             &sk.E.p, &sk.E.a, &sk.E.b, &mpi_g, &sk.E.n,
                             &sk.E.h, &mpi_q, &sk.d, NULL);
  else
    rc = sexp_extract_param (keyparms, NULL, "/q?+d",
                             &mpi_q, &sk.d, NULL);
  if (rc)
    goto leave;
  if (mpi_g)
    {
      point_init (&sk.E.G);
      rc = _gcry_ecc_os2ec (&sk.E.G, mpi_g);
      if (rc)
        goto leave;
    }
  /* Add missing parameters using the optional curve parameter.  */
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
  /* Guess required fields if a curve parameter has not been given.
     FIXME: This is a crude hacks.  We need to fix that.  */
  if (!curvename)
    {
      sk.E.model = ((ctx.flags & PUBKEY_FLAG_EDDSA)
                    ? MPI_EC_EDWARDS
                    : MPI_EC_WEIERSTRASS);
      sk.E.dialect = ((ctx.flags & PUBKEY_FLAG_EDDSA)
                      ? ECC_DIALECT_ED25519
                      : ECC_DIALECT_STANDARD);
      if (!sk.E.h)
	sk.E.h = mpi_const (MPI_C_ONE);
    }
  if (DBG_CIPHER)
    {
      log_debug ("ecc_sign   info: %s/%s%s\n",
                 _gcry_ecc_model2str (sk.E.model),
                 _gcry_ecc_dialect2str (sk.E.dialect),
                 (ctx.flags & PUBKEY_FLAG_EDDSA)? "+EdDSA":"");
      if (sk.E.name)
        log_debug  ("ecc_sign   name: %s\n", sk.E.name);
      log_printmpi ("ecc_sign      p", sk.E.p);
      log_printmpi ("ecc_sign      a", sk.E.a);
      log_printmpi ("ecc_sign      b", sk.E.b);
      log_printpnt ("ecc_sign    g",   &sk.E.G, NULL);
      log_printmpi ("ecc_sign      n", sk.E.n);
      log_printmpi ("ecc_sign      h", sk.E.h);
      log_printmpi ("ecc_sign      q", mpi_q);
      if (!fips_mode ())
        log_printmpi ("ecc_sign      d", sk.d);
    }
  if (!sk.E.p || !sk.E.a || !sk.E.b || !sk.E.G.x || !sk.E.n || !sk.E.h || !sk.d)
    {
      rc = GPG_ERR_NO_OBJ;
      goto leave;
    }


  sig_r = mpi_new (0);
  sig_s = mpi_new (0);
  if ((ctx.flags & PUBKEY_FLAG_EDDSA))
    {
      /* EdDSA requires the public key.  */
      rc = _gcry_ecc_eddsa_sign (data, &sk, sig_r, sig_s, ctx.hash_algo, mpi_q);
      if (!rc)
        rc = sexp_build (r_sig, NULL,
                         "(sig-val(eddsa(r%M)(s%M)))", sig_r, sig_s);
    }
  else if ((ctx.flags & PUBKEY_FLAG_GOST))
    {
      rc = _gcry_ecc_gost_sign (data, &sk, sig_r, sig_s);
      if (!rc)
        rc = sexp_build (r_sig, NULL,
                         "(sig-val(gost(r%M)(s%M)))", sig_r, sig_s);
    }
  else
    {
      rc = _gcry_ecc_ecdsa_sign (data, &sk, sig_r, sig_s,
                                 ctx.flags, ctx.hash_algo);
      if (!rc)
        rc = sexp_build (r_sig, NULL,
                         "(sig-val(ecdsa(r%M)(s%M)))", sig_r, sig_s);
    }


 leave:
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
  _gcry_mpi_release (sig_r);
  _gcry_mpi_release (sig_s);
  xfree (curvename);
  _gcry_mpi_release (data);
  sexp_release (l1);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("ecc_sign      => %s\n", gpg_strerror (rc));
  return rc;
}