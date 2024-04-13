ecc_encrypt_raw (gcry_sexp_t *r_ciph, gcry_sexp_t s_data, gcry_sexp_t keyparms)
{
  unsigned int nbits;
  gcry_err_code_t rc;
  struct pk_encoding_ctx ctx;
  gcry_sexp_t l1 = NULL;
  char *curvename = NULL;
  gcry_mpi_t mpi_g = NULL;
  gcry_mpi_t mpi_q = NULL;
  gcry_mpi_t mpi_s = NULL;
  gcry_mpi_t mpi_e = NULL;
  gcry_mpi_t data = NULL;
  ECC_public_key pk;
  mpi_ec_t ec = NULL;
  int flags = 0;

  memset (&pk, 0, sizeof pk);
  _gcry_pk_util_init_encoding_ctx (&ctx, PUBKEY_OP_ENCRYPT,
                                   (nbits = ecc_get_nbits (keyparms)));

  /* Look for flags. */
  l1 = sexp_find_token (keyparms, "flags", 0);
  if (l1)
    {
      rc = _gcry_pk_util_parse_flaglist (l1, &flags, NULL);
      if (rc)
        goto leave;
    }
  sexp_release (l1);
  l1 = NULL;

  /*
   * Extract the data.
   */
  rc = _gcry_pk_util_data_to_mpi (s_data, &data, &ctx);
  if (rc)
    goto leave;
  if (mpi_is_opaque (data))
    {
      rc = GPG_ERR_INV_DATA;
      goto leave;
    }

  /*
   * Extract the key.
   */
  rc = sexp_extract_param (keyparms, NULL,
                           (flags & PUBKEY_FLAG_DJB_TWEAK)?
                           "-p?a?b?g?n?h?/q" : "-p?a?b?g?n?h?+q",
                           &pk.E.p, &pk.E.a, &pk.E.b, &mpi_g, &pk.E.n, &pk.E.h,
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
  l1 = sexp_find_token (keyparms, "curve", 5);
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
  /* Guess required fields if a curve parameter has not been given.  */
  if (!curvename)
    {
      pk.E.model = MPI_EC_WEIERSTRASS;
      pk.E.dialect = ECC_DIALECT_STANDARD;
      if (!pk.E.h)
	pk.E.h = mpi_const (MPI_C_ONE);
    }

  /*
   * Tweak the scalar bits by cofactor and number of bits of the field.
   * It assumes the cofactor is a power of 2.
   */
  if ((flags & PUBKEY_FLAG_DJB_TWEAK))
    {
      int i;

      for (i = 0; i < mpi_get_nbits (pk.E.h) - 1; i++)
        mpi_clear_bit (data, i);
      mpi_set_highbit (data, mpi_get_nbits (pk.E.p) - 1);
    }
  if (DBG_CIPHER)
    log_mpidump ("ecc_encrypt data", data);

  if (DBG_CIPHER)
    {
      log_debug ("ecc_encrypt info: %s/%s\n",
                 _gcry_ecc_model2str (pk.E.model),
                 _gcry_ecc_dialect2str (pk.E.dialect));
      if (pk.E.name)
        log_debug  ("ecc_encrypt name: %s\n", pk.E.name);
      log_printmpi ("ecc_encrypt    p", pk.E.p);
      log_printmpi ("ecc_encrypt    a", pk.E.a);
      log_printmpi ("ecc_encrypt    b", pk.E.b);
      log_printpnt ("ecc_encrypt  g",   &pk.E.G, NULL);
      log_printmpi ("ecc_encrypt    n", pk.E.n);
      log_printmpi ("ecc_encrypt    h", pk.E.h);
      log_printmpi ("ecc_encrypt    q", mpi_q);
    }
  if (!pk.E.p || !pk.E.a || !pk.E.b || !pk.E.G.x || !pk.E.n || !pk.E.h || !mpi_q)
    {
      rc = GPG_ERR_NO_OBJ;
      goto leave;
    }

  /* Compute the encrypted value.  */
  ec = _gcry_mpi_ec_p_internal_new (pk.E.model, pk.E.dialect, flags,
                                    pk.E.p, pk.E.a, pk.E.b);

  /* Convert the public key.  */
  if (mpi_q)
    {
      point_init (&pk.Q);
      if (ec->model == MPI_EC_MONTGOMERY)
        rc = _gcry_ecc_mont_decodepoint (mpi_q, ec, &pk.Q);
      else
        rc = _gcry_ecc_os2ec (&pk.Q, mpi_q);
      if (rc)
        goto leave;
    }

  /* The following is false: assert( mpi_cmp_ui( R.x, 1 )==0 );, so */
  {
    mpi_point_struct R;  /* Result that we return.  */
    gcry_mpi_t x, y;
    unsigned char *rawmpi;
    unsigned int rawmpilen;

    x = mpi_new (0);
    if (ec->model == MPI_EC_MONTGOMERY)
      y = NULL;
    else
      y = mpi_new (0);

    point_init (&R);

    /* R = kQ  <=>  R = kdG  */
    _gcry_mpi_ec_mul_point (&R, data, &pk.Q, ec);

    if (_gcry_mpi_ec_get_affine (x, y, &R, ec))
      {
        /*
         * Here, X is 0.  In the X25519 computation on Curve25519, X0
         * function maps infinity to zero.  So, when PUBKEY_FLAG_DJB_TWEAK
         * is enabled, return the result of 0 not raising an error.
         *
         * This is a corner case.  It never occurs with properly
         * generated public keys, but it might happen with blindly
         * imported public key which might not follow the key
         * generation procedure.
         */
        if (!(flags & PUBKEY_FLAG_DJB_TWEAK))
          { /* It's not for X25519, then, the input data was simply wrong.  */
            rc = GPG_ERR_INV_DATA;
            goto leave;
          }
      }
    if (y)
      mpi_s = _gcry_ecc_ec2os (x, y, pk.E.p);
    else
      {
        rawmpi = _gcry_mpi_get_buffer_extra (x, nbits/8, -1, &rawmpilen, NULL);
        if (!rawmpi)
          rc = gpg_err_code_from_syserror ();
        else
          {
            rawmpi[0] = 0x40;
            rawmpilen++;
            mpi_s = mpi_new (0);
            mpi_set_opaque (mpi_s, rawmpi, rawmpilen*8);
          }
      }

    /* R = kG */
    _gcry_mpi_ec_mul_point (&R, data, &pk.E.G, ec);

    if (_gcry_mpi_ec_get_affine (x, y, &R, ec))
      {
        rc = GPG_ERR_INV_DATA;
        goto leave;
      }
    if (y)
      mpi_e = _gcry_ecc_ec2os (x, y, pk.E.p);
    else
      {
        rawmpi = _gcry_mpi_get_buffer_extra (x, nbits/8, -1, &rawmpilen, NULL);
        if (!rawmpi)
          rc = gpg_err_code_from_syserror ();
        else
          {
            rawmpi[0] = 0x40;
            rawmpilen++;
            mpi_e = mpi_new (0);
            mpi_set_opaque (mpi_e, rawmpi, rawmpilen*8);
          }
      }


    mpi_free (x);
    mpi_free (y);

    point_free (&R);
  }

  if (!rc)
    rc = sexp_build (r_ciph, NULL, "(enc-val(ecdh(s%m)(e%m)))", mpi_s, mpi_e);

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
  _gcry_mpi_release (mpi_s);
  _gcry_mpi_release (mpi_e);
  xfree (curvename);
  sexp_release (l1);
  _gcry_mpi_ec_free (ec);
  _gcry_pk_util_free_encoding_ctx (&ctx);
  if (DBG_CIPHER)
    log_debug ("ecc_encrypt    => %s\n", gpg_strerror (rc));
  return rc;
}