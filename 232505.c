ecc_generate (const gcry_sexp_t genparms, gcry_sexp_t *r_skey)
{
  gpg_err_code_t rc;
  unsigned int nbits;
  elliptic_curve_t E;
  ECC_secret_key sk;
  gcry_mpi_t Gx = NULL;
  gcry_mpi_t Gy = NULL;
  gcry_mpi_t Qx = NULL;
  gcry_mpi_t Qy = NULL;
  char *curve_name = NULL;
  gcry_sexp_t l1;
  mpi_ec_t ctx = NULL;
  gcry_sexp_t curve_info = NULL;
  gcry_sexp_t curve_flags = NULL;
  gcry_mpi_t base = NULL;
  gcry_mpi_t public = NULL;
  gcry_mpi_t secret = NULL;
  int flags = 0;

  memset (&E, 0, sizeof E);
  memset (&sk, 0, sizeof sk);

  rc = _gcry_pk_util_get_nbits (genparms, &nbits);
  if (rc)
    return rc;

  /* Parse the optional "curve" parameter. */
  l1 = sexp_find_token (genparms, "curve", 0);
  if (l1)
    {
      curve_name = _gcry_sexp_nth_string (l1, 1);
      sexp_release (l1);
      if (!curve_name)
        return GPG_ERR_INV_OBJ; /* No curve name or value too large. */
    }

  /* Parse the optional flags list.  */
  l1 = sexp_find_token (genparms, "flags", 0);
  if (l1)
    {
      rc = _gcry_pk_util_parse_flaglist (l1, &flags, NULL);
      sexp_release (l1);
      if (rc)
        goto leave;
    }

  /* Parse the deprecated optional transient-key flag.  */
  l1 = sexp_find_token (genparms, "transient-key", 0);
  if (l1)
    {
      flags |= PUBKEY_FLAG_TRANSIENT_KEY;
      sexp_release (l1);
    }

  /* NBITS is required if no curve name has been given.  */
  if (!nbits && !curve_name)
    return GPG_ERR_NO_OBJ; /* No NBITS parameter. */

  rc = _gcry_ecc_fill_in_curve (nbits, curve_name, &E, &nbits);
  if (rc)
    goto leave;

  if (DBG_CIPHER)
    {
      log_debug ("ecgen curve info: %s/%s\n",
                 _gcry_ecc_model2str (E.model),
                 _gcry_ecc_dialect2str (E.dialect));
      if (E.name)
        log_debug ("ecgen curve used: %s\n", E.name);
      log_printmpi ("ecgen curve   p", E.p);
      log_printmpi ("ecgen curve   a", E.a);
      log_printmpi ("ecgen curve   b", E.b);
      log_printmpi ("ecgen curve   n", E.n);
      log_printmpi ("ecgen curve   h", E.h);
      log_printpnt ("ecgen curve G", &E.G, NULL);
    }

  ctx = _gcry_mpi_ec_p_internal_new (E.model, E.dialect, flags, E.p, E.a, E.b);

  if (E.model == MPI_EC_MONTGOMERY)
    rc = nist_generate_key (&sk, &E, ctx, flags, nbits, &Qx, NULL);
  else if ((flags & PUBKEY_FLAG_EDDSA))
    rc = _gcry_ecc_eddsa_genkey (&sk, &E, ctx, flags);
  else
    rc = nist_generate_key (&sk, &E, ctx, flags, nbits, &Qx, &Qy);
  if (rc)
    goto leave;

  /* Copy data to the result.  */
  Gx = mpi_new (0);
  Gy = mpi_new (0);
  if (E.model != MPI_EC_MONTGOMERY)
    {
      if (_gcry_mpi_ec_get_affine (Gx, Gy, &sk.E.G, ctx))
        log_fatal ("ecgen: Failed to get affine coordinates for %s\n", "G");
      base = _gcry_ecc_ec2os (Gx, Gy, sk.E.p);
    }
  if ((sk.E.dialect == ECC_DIALECT_ED25519 || E.model == MPI_EC_MONTGOMERY)
      && !(flags & PUBKEY_FLAG_NOCOMP))
    {
      unsigned char *encpk;
      unsigned int encpklen;

      if (E.model != MPI_EC_MONTGOMERY)
        /* (Gx and Gy are used as scratch variables)  */
        rc = _gcry_ecc_eddsa_encodepoint (&sk.Q, ctx, Gx, Gy,
                                          !!(flags & PUBKEY_FLAG_COMP),
                                          &encpk, &encpklen);
      else
        {
          encpk = _gcry_mpi_get_buffer_extra (Qx, nbits/8,
                                              -1, &encpklen, NULL);
          if (encpk == NULL)
            rc = gpg_err_code_from_syserror ();
          else
            {
              encpk[0] = 0x40;
              encpklen++;
            }
        }
      if (rc)
        goto leave;
      public = mpi_new (0);
      mpi_set_opaque (public, encpk, encpklen*8);
    }
  else
    {
      if (!Qx)
        {
          /* This is the case for a key from _gcry_ecc_eddsa_generate
             with no compression.  */
          Qx = mpi_new (0);
          Qy = mpi_new (0);
          if (_gcry_mpi_ec_get_affine (Qx, Qy, &sk.Q, ctx))
            log_fatal ("ecgen: Failed to get affine coordinates for %s\n", "Q");
        }
      public = _gcry_ecc_ec2os (Qx, Qy, sk.E.p);
    }
  secret = sk.d; sk.d = NULL;
  if (E.name)
    {
      rc = sexp_build (&curve_info, NULL, "(curve %s)", E.name);
      if (rc)
        goto leave;
    }

  if ((flags & PUBKEY_FLAG_PARAM) || (flags & PUBKEY_FLAG_EDDSA)
      || (flags & PUBKEY_FLAG_DJB_TWEAK))
    {
      rc = sexp_build
        (&curve_flags, NULL,
         ((flags & PUBKEY_FLAG_PARAM) && (flags & PUBKEY_FLAG_EDDSA))?
         "(flags param eddsa)" :
         ((flags & PUBKEY_FLAG_PARAM) && (flags & PUBKEY_FLAG_EDDSA))?
         "(flags param djb-tweak)" :
         ((flags & PUBKEY_FLAG_PARAM))?
         "(flags param)" : ((flags & PUBKEY_FLAG_EDDSA))?
         "(flags eddsa)" : "(flags djb-tweak)" );
      if (rc)
        goto leave;
    }

  if ((flags & PUBKEY_FLAG_PARAM) && E.name)
    rc = sexp_build (r_skey, NULL,
                     "(key-data"
                     " (public-key"
                     "  (ecc%S%S(p%m)(a%m)(b%m)(g%m)(n%m)(h%m)(q%m)))"
                     " (private-key"
                     "  (ecc%S%S(p%m)(a%m)(b%m)(g%m)(n%m)(h%m)(q%m)(d%m)))"
                     " )",
                     curve_info, curve_flags,
                     sk.E.p, sk.E.a, sk.E.b, base, sk.E.n, sk.E.h, public,
                     curve_info, curve_flags,
                     sk.E.p, sk.E.a, sk.E.b, base, sk.E.n, sk.E.h, public,
                                                                   secret);
  else
    rc = sexp_build (r_skey, NULL,
                     "(key-data"
                     " (public-key"
                     "  (ecc%S%S(q%m)))"
                     " (private-key"
                     "  (ecc%S%S(q%m)(d%m)))"
                     " )",
                     curve_info, curve_flags,
                     public,
                     curve_info, curve_flags,
                     public, secret);
  if (rc)
    goto leave;

  if (DBG_CIPHER)
    {
      log_printmpi ("ecgen result  p", sk.E.p);
      log_printmpi ("ecgen result  a", sk.E.a);
      log_printmpi ("ecgen result  b", sk.E.b);
      log_printmpi ("ecgen result  G", base);
      log_printmpi ("ecgen result  n", sk.E.n);
      log_printmpi ("ecgen result  h", sk.E.h);
      log_printmpi ("ecgen result  Q", public);
      log_printmpi ("ecgen result  d", secret);
      if ((flags & PUBKEY_FLAG_EDDSA))
        log_debug ("ecgen result  using Ed25519+EdDSA\n");
    }

 leave:
  mpi_free (secret);
  mpi_free (public);
  mpi_free (base);
  {
    _gcry_ecc_curve_free (&sk.E);
    point_free (&sk.Q);
    mpi_free (sk.d);
  }
  _gcry_ecc_curve_free (&E);
  mpi_free (Gx);
  mpi_free (Gy);
  mpi_free (Qx);
  mpi_free (Qy);
  _gcry_mpi_ec_free (ctx);
  xfree (curve_name);
  sexp_release (curve_flags);
  sexp_release (curve_info);
  return rc;
}