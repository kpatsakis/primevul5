test_ecdh_only_keys (ECC_secret_key *sk, unsigned int nbits, int flags)
{
  ECC_public_key pk;
  gcry_mpi_t test;
  mpi_point_struct R_;
  gcry_mpi_t x0, x1;
  mpi_ec_t ec;

  if (DBG_CIPHER)
    log_debug ("Testing ECDH only key.\n");

  point_init (&R_);

  pk.E = _gcry_ecc_curve_copy (sk->E);
  point_init (&pk.Q);
  point_set (&pk.Q, &sk->Q);

  if ((flags & PUBKEY_FLAG_DJB_TWEAK))
    {
      char *rndbuf;

      test = mpi_new (256);
      rndbuf = _gcry_random_bytes (32, GCRY_WEAK_RANDOM);
      rndbuf[0] &= 0x7f;  /* Clear bit 255. */
      rndbuf[0] |= 0x40;  /* Set bit 254.   */
      rndbuf[31] &= 0xf8; /* Clear bits 2..0 so that d mod 8 == 0  */
      _gcry_mpi_set_buffer (test, rndbuf, 32, 0);
      xfree (rndbuf);
    }
  else
    {
      test = mpi_new (nbits);
      _gcry_mpi_randomize (test, nbits, GCRY_WEAK_RANDOM);
    }

  ec = _gcry_mpi_ec_p_internal_new (pk.E.model, pk.E.dialect, flags,
                                    pk.E.p, pk.E.a, pk.E.b);
  x0 = mpi_new (0);
  x1 = mpi_new (0);

  /* R_ = hkQ  <=>  R_ = hkdG  */
  _gcry_mpi_ec_mul_point (&R_, test, &pk.Q, ec);
  if (!(flags & PUBKEY_FLAG_DJB_TWEAK))
    _gcry_mpi_ec_mul_point (&R_, ec->h, &R_, ec);
  if (_gcry_mpi_ec_get_affine (x0, NULL, &R_, ec))
    log_fatal ("ecdh: Failed to get affine coordinates for hkQ\n");

  _gcry_mpi_ec_mul_point (&R_, test, &pk.E.G, ec);
  _gcry_mpi_ec_mul_point (&R_, sk->d, &R_, ec);
  /* R_ = hdkG */
  if (!(flags & PUBKEY_FLAG_DJB_TWEAK))
    _gcry_mpi_ec_mul_point (&R_, ec->h, &R_, ec);

  if (_gcry_mpi_ec_get_affine (x1, NULL, &R_, ec))
    log_fatal ("ecdh: Failed to get affine coordinates for hdkG\n");

  if (mpi_cmp (x0, x1))
    {
      log_fatal ("ECDH test failed.\n");
    }

  mpi_free (x0);
  mpi_free (x1);
  _gcry_mpi_ec_free (ec);

  point_free (&pk.Q);
  _gcry_ecc_curve_free (&pk.E);

  point_free (&R_);
  mpi_free (test);
}