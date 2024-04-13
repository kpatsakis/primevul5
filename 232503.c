ecc_verify (gcry_sexp_t s_sig, gcry_sexp_t s_data, gcry_sexp_t s_keyparms)
{
  gcry_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_sexp_t l1 = NULL;
  char *curvename = NULL;
  gcry_mpi_t mpi_g = NULL;
  gcry_mpi_t mpi_q = NULL;
  gcry_mpi_t sig_r = NULL;
  gcry_mpi_t sig_s = NULL;
  gcry_mpi_t data = NULL;
  ECC_public_key pk;
  int sigflags;

  memset (&pk, 0, sizeof pk);
  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_VERIFY,
                                   ecc_get_nbits (s_keyparms));

  /* Extract the data.  */
  rc = _gcry_pk_util_data_to_mpi (s_data, &data, &ctx);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    log_mpidump ("ecc_verify data", data);

  /*
   * Extract the signature value.
   */
  rc = _gcry_pk_util_preparse_sigval (s_sig, ecc_names, &l1, &sigflags);
  if (rc)
    goto leave;
  rc = sexp_extract_param (l1, NULL, (sigflags & PUBKEY_FLAG_EDDSA)? "/rs":"rs",
                           &sig_r, &sig_s, NULL);
  if (rc)
    goto leave;
  if (DBG_CIPHER)
    {
      log_mpidump ("ecc_verify  s_r", sig_r);
      log_mpidump ("ecc_verify  s_s", sig_s);
    }
  if ((ctx.flags & PUBKEY_FLAG_EDDSA) ^ (sigflags & PUBKEY_FLAG_EDDSA))
    {
      rc = GPG_ERR_CONFLICT; /* Inconsistent use of flag/algoname.  */
      goto leave;
    }


  /*
   * Extract the key.
   */
  if ((ctx.flags & PUBKEY_FLAG_PARAM))
    rc = sexp_extract_param (s_keyparms, NULL, "-p?a?b?g?n?h?/q",
                             &pk.E.p, &pk.E.a, &pk.E.b, &mpi_g, &pk.E.n,
                             &pk.E.h, &mpi_q, NULL);
  else
    rc = sexp_extract_param (s_keyparms, NULL, "/q",
                             &mpi_q, NULL);
  if (rc)
    goto leave;
  if (mpi_g)
    {
      point_init (&pk.E.G);
      rc = _gcry_ecc_os2ec (&pk.E.G, mpi_g);
      if (rc)
        goto leave;
    }
  /* Add missing parameters using the optional curve parameter.  */
  sexp_release (l1);
  l1 = sexp_find_token (s_keyparms, "curve", 5);
  if (l1)
    {
      curvename = sexp_nth_string (l1, 1);
      if (curvename)
        {
          rc = _gcry_ecc_fill_in_curve (0, curvename, &pk.E, NULL);
          if (rc)
            goto leave;
        }
    }
  /* Guess required fields if a curve parameter has not been given.
     FIXME: This is a crude hacks.  We need to fix that.  */
  if (!curvename)
    {
      pk.E.model = ((sigflags & PUBKEY_FLAG_EDDSA)
                    ? MPI_EC_EDWARDS
                    : MPI_EC_WEIERSTRASS);
      pk.E.dialect = ((sigflags & PUBKEY_FLAG_EDDSA)
                      ? ECC_DIALECT_ED25519
                      : ECC_DIALECT_STANDARD);
      if (!pk.E.h)
	pk.E.h = mpi_const (MPI_C_ONE);
    }

  if (DBG_CIPHER)
    {
      log_debug ("ecc_verify info: %s/%s%s\n",
                 _gcry_ecc_model2str (pk.E.model),
                 _gcry_ecc_dialect2str (pk.E.dialect),
                 (sigflags & PUBKEY_FLAG_EDDSA)? "+EdDSA":"");
      if (pk.E.name)
        log_debug  ("ecc_verify name: %s\n", pk.E.name);
      log_printmpi ("ecc_verify    p", pk.E.p);
      log_printmpi ("ecc_verify    a", pk.E.a);
      log_printmpi ("ecc_verify    b", pk.E.b);
      log_printpnt ("ecc_verify  g",   &pk.E.G, NULL);
      log_printmpi ("ecc_verify    n", pk.E.n);
      log_printmpi ("ecc_verify    h", pk.E.h);
      log_printmpi ("ecc_verify    q", mpi_q);
    }
  if (!pk.E.p || !pk.E.a || !pk.E.b || !pk.E.G.x || !pk.E.n || !pk.E.h || !mpi_q)
    {
      rc = GPG_ERR_NO_OBJ;
      goto leave;
    }


  /*
   * Verify the signature.
   */
  if ((sigflags & PUBKEY_FLAG_EDDSA))
    {
      rc = _gcry_ecc_eddsa_verify (data, &pk, sig_r, sig_s,
                                   ctx.hash_algo, mpi_q);
    }
  else if ((sigflags & PUBKEY_FLAG_GOST))
    {
      point_init (&pk.Q);
      rc = _gcry_ecc_os2ec (&pk.Q, mpi_q);
      if (rc)
        goto leave;

      rc = _gcry_ecc_gost_verify (data, &pk, sig_r, sig_s);
    }
  else
    {
      point_init (&pk.Q);
      if (pk.E.dialect == ECC_DIALECT_ED25519)
        {
          mpi_ec_t ec;

          /* Fixme: Factor the curve context setup out of eddsa_verify
             and ecdsa_verify. So that we don't do it twice.  */
          ec = _gcry_mpi_ec_p_internal_new (pk.E.model, pk.E.dialect, 0,
                                            pk.E.p, pk.E.a, pk.E.b);

          rc = _gcry_ecc_eddsa_decodepoint (mpi_q, ec, &pk.Q, NULL, NULL);
          _gcry_mpi_ec_free (ec);
        }
      else
        {
          rc = _gcry_ecc_os2ec (&pk.Q, mpi_q);
        }
      if (rc)
        goto leave;

      if (mpi_is_opaque (data))
        {
          const void *abuf;
          unsigned int abits, qbits;
          gcry_mpi_t a;

          qbits = mpi_get_nbits (pk.E.n);

          abuf = mpi_get_opaque (data, &abits);
          rc = _gcry_mpi_scan (&a, GCRYMPI_FMT_USG, abuf, (abits+7)/8, NULL);
          if (!rc)
            {
              if (abits > qbits)
                mpi_rshift (a, a, abits - qbits);

              rc = _gcry_ecc_ecdsa_verify (a, &pk, sig_r, sig_s);
              _gcry_mpi_release (a);
            }
        }
      else
        rc = _gcry_ecc_ecdsa_verify (data, &pk, sig_r, sig_s);
    }

 leave:
  _gcry_mpi_release (pk.E.p);
  _gcry_mpi_release (pk.E.a);
  _gcry_mpi_release (pk.E.b);
  _gcry_mpi_release (mpi_g);
  point_free (&pk.E.G);
  _gcry_mpi_release (pk.E.n);
  _gcry_mpi_release (pk.E.h);
  _gcry_mpi_release (mpi_q);
  point_free (&pk.Q);
  _gcry_mpi_release (data);
  _gcry_mpi_release (sig_r);
  _gcry_mpi_release (sig_s);
  xfree (curvename);
  sexp_release (l1);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("ecc_verify    => %s\n", rc?gpg_strerror (rc):"Good");
  return rc;
}