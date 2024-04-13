nist_generate_key (ECC_secret_key *sk, elliptic_curve_t *E, mpi_ec_t ctx,
                   int flags, unsigned int nbits,
                   gcry_mpi_t *r_x, gcry_mpi_t *r_y)
{
  mpi_point_struct Q;
  gcry_random_level_t random_level;
  gcry_mpi_t x, y;
  const unsigned int pbits = mpi_get_nbits (E->p);

  point_init (&Q);

  if ((flags & PUBKEY_FLAG_TRANSIENT_KEY))
    random_level = GCRY_STRONG_RANDOM;
  else
    random_level = GCRY_VERY_STRONG_RANDOM;

  /* Generate a secret.  */
  if (ctx->dialect == ECC_DIALECT_ED25519 || (flags & PUBKEY_FLAG_DJB_TWEAK))
    {
      char *rndbuf;

      sk->d = mpi_snew (256);
      rndbuf = _gcry_random_bytes_secure (32, random_level);
      rndbuf[0] &= 0x7f;  /* Clear bit 255. */
      rndbuf[0] |= 0x40;  /* Set bit 254.   */
      rndbuf[31] &= 0xf8; /* Clear bits 2..0 so that d mod 8 == 0  */
      _gcry_mpi_set_buffer (sk->d, rndbuf, 32, 0);
      xfree (rndbuf);
    }
  else
    sk->d = _gcry_dsa_gen_k (E->n, random_level);


  /* Compute Q.  */
  _gcry_mpi_ec_mul_point (&Q, sk->d, &E->G, ctx);

  /* Copy the stuff to the key structures. */
  sk->E.model = E->model;
  sk->E.dialect = E->dialect;
  sk->E.p = mpi_copy (E->p);
  sk->E.a = mpi_copy (E->a);
  sk->E.b = mpi_copy (E->b);
  point_init (&sk->E.G);
  point_set (&sk->E.G, &E->G);
  sk->E.n = mpi_copy (E->n);
  sk->E.h = mpi_copy (E->h);
  point_init (&sk->Q);

  x = mpi_new (pbits);
  if (r_y == NULL)
    y = NULL;
  else
    y = mpi_new (pbits);
  if (_gcry_mpi_ec_get_affine (x, y, &Q, ctx))
    log_fatal ("ecgen: Failed to get affine coordinates for %s\n", "Q");

  /* We want the Q=(x,y) be a "compliant key" in terms of the
   * http://tools.ietf.org/html/draft-jivsov-ecc-compact, which simply
   * means that we choose either Q=(x,y) or -Q=(x,p-y) such that we
   * end up with the min(y,p-y) as the y coordinate.  Such a public
   * key allows the most efficient compression: y can simply be
   * dropped because we know that it's a minimum of the two
   * possibilities without any loss of security.  Note that we don't
   * do that for Ed25519 so that we do not violate the special
   * construction of the secret key.  */
  if (r_y == NULL || E->dialect == ECC_DIALECT_ED25519)
    point_set (&sk->Q, &Q);
  else
    {
      gcry_mpi_t negative;

      negative = mpi_new (pbits);

      if (E->model == MPI_EC_WEIERSTRASS)
        mpi_sub (negative, E->p, y);      /* negative = p - y */
      else
        mpi_sub (negative, E->p, x);      /* negative = p - x */

      if (mpi_cmp (negative, y) < 0)   /* p - y < p */
        {
          /* We need to end up with -Q; this assures that new Q's y is
             the smallest one */
          if (E->model == MPI_EC_WEIERSTRASS)
            {
              mpi_free (y);
              y = negative;
            }
          else
            {
              mpi_free (x);
              x = negative;
            }
          mpi_sub (sk->d, E->n, sk->d);   /* d = order - d */
          mpi_point_set (&sk->Q, x, y, mpi_const (MPI_C_ONE));

          if (DBG_CIPHER)
            log_debug ("ecgen converted Q to a compliant point\n");
        }
      else /* p - y >= p */
        {
          /* No change is needed exactly 50% of the time: just copy. */
          mpi_free (negative);
          point_set (&sk->Q, &Q);
          if (DBG_CIPHER)
            log_debug ("ecgen didn't need to convert Q to a compliant point\n");
        }
    }

  *r_x = x;
  if (r_y)
    *r_y = y;

  point_free (&Q);
  /* Now we can test our keys (this should never fail!).  */
  if ((flags & PUBKEY_FLAG_NO_KEYTEST))
    ; /* User requested to skip the test.  */
  else if (sk->E.model != MPI_EC_MONTGOMERY)
    test_keys (sk, nbits - 64);
  else
    test_ecdh_only_keys (sk, nbits - 64, flags);

  return 0;
}