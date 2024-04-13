check_secret_key (ECC_secret_key *sk, mpi_ec_t ec, int flags)
{
  int rc = 1;
  mpi_point_struct Q;
  gcry_mpi_t x1, y1;
  gcry_mpi_t x2 = NULL;
  gcry_mpi_t y2 = NULL;

  point_init (&Q);
  x1 = mpi_new (0);
  if (ec->model == MPI_EC_MONTGOMERY)
    y1 = NULL;
  else
    y1 = mpi_new (0);

  /* G in E(F_p) */
  if (!_gcry_mpi_ec_curve_point (&sk->E.G, ec))
    {
      if (DBG_CIPHER)
        log_debug ("Bad check: Point 'G' does not belong to curve 'E'!\n");
      goto leave;
    }

  /* G != PaI */
  if (!mpi_cmp_ui (sk->E.G.z, 0))
    {
      if (DBG_CIPHER)
        log_debug ("Bad check: 'G' cannot be Point at Infinity!\n");
      goto leave;
    }

  /* Check order of curve.  */
  if (sk->E.dialect != ECC_DIALECT_ED25519 && !(flags & PUBKEY_FLAG_DJB_TWEAK))
    {
      _gcry_mpi_ec_mul_point (&Q, sk->E.n, &sk->E.G, ec);
      if (mpi_cmp_ui (Q.z, 0))
        {
          if (DBG_CIPHER)
            log_debug ("check_secret_key: E is not a curve of order n\n");
          goto leave;
        }
    }

  /* Pubkey cannot be PaI */
  if (!mpi_cmp_ui (sk->Q.z, 0))
    {
      if (DBG_CIPHER)
        log_debug ("Bad check: Q can not be a Point at Infinity!\n");
      goto leave;
    }

  /* pubkey = [d]G over E */
  if (!_gcry_ecc_compute_public (&Q, ec, &sk->E.G, sk->d))
    {
      if (DBG_CIPHER)
        log_debug ("Bad check: computation of dG failed\n");
      goto leave;
    }
  if (_gcry_mpi_ec_get_affine (x1, y1, &Q, ec))
    {
      if (DBG_CIPHER)
        log_debug ("Bad check: Q can not be a Point at Infinity!\n");
      goto leave;
    }

  if ((flags & PUBKEY_FLAG_EDDSA))
    ; /* Fixme: EdDSA is special.  */
  else if (!mpi_cmp_ui (sk->Q.z, 1))
    {
      /* Fast path if Q is already in affine coordinates.  */
      if (mpi_cmp (x1, sk->Q.x) || (y1 && mpi_cmp (y1, sk->Q.y)))
        {
          if (DBG_CIPHER)
            log_debug
              ("Bad check: There is NO correspondence between 'd' and 'Q'!\n");
          goto leave;
        }
    }
  else
    {
      x2 = mpi_new (0);
      y2 = mpi_new (0);
      if (_gcry_mpi_ec_get_affine (x2, y2, &sk->Q, ec))
        {
          if (DBG_CIPHER)
            log_debug ("Bad check: Q can not be a Point at Infinity!\n");
          goto leave;
        }

      if (mpi_cmp (x1, x2) || mpi_cmp (y1, y2))
        {
          if (DBG_CIPHER)
            log_debug
              ("Bad check: There is NO correspondence between 'd' and 'Q'!\n");
          goto leave;
        }
    }
  rc = 0; /* Okay.  */

 leave:
  mpi_free (x2);
  mpi_free (x1);
  mpi_free (y1);
  mpi_free (y2);
  point_free (&Q);
  return rc;
}