compute_keygrip (gcry_md_hd_t md, gcry_sexp_t keyparms)
{
#define N_COMPONENTS 7
  static const char names[N_COMPONENTS] = "pabgnhq";
  gpg_err_code_t rc;
  gcry_sexp_t l1;
  gcry_mpi_t values[N_COMPONENTS];
  int idx;
  char *curvename = NULL;
  int flags = 0;
  enum gcry_mpi_ec_models model = 0;
  enum ecc_dialects dialect = 0;

  /* Clear the values first.  */
  for (idx=0; idx < N_COMPONENTS; idx++)
    values[idx] = NULL;


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
    {
      if ((flags & PUBKEY_FLAG_DJB_TWEAK))
        rc = sexp_extract_param (keyparms, NULL, "p?a?b?g?n?h?/q",
                                 &values[0], &values[1], &values[2],
                                 &values[3], &values[4], &values[5],
                                 &values[6], NULL);
      else
        rc = sexp_extract_param (keyparms, NULL, "p?a?b?g?n?h?q",
                                 &values[0], &values[1], &values[2],
                                 &values[3], &values[4], &values[5],
                                 &values[6], NULL);
    }
  else
    {
      if ((flags & PUBKEY_FLAG_DJB_TWEAK))
        rc = sexp_extract_param (keyparms, NULL, "/q",
                                 &values[6], NULL);
      else
        rc = sexp_extract_param (keyparms, NULL, "q",
                                 &values[6], NULL);
    }
  if (rc)
    goto leave;

  /* Check whether a curve parameter is available and use that to fill
     in missing values.  */
  sexp_release (l1);
  l1 = sexp_find_token (keyparms, "curve", 5);
  if (l1)
    {
      curvename = sexp_nth_string (l1, 1);
      if (curvename)
        {
          rc = _gcry_ecc_update_curve_param (curvename,
                                             &model, &dialect,
                                             &values[0], &values[1], &values[2],
                                             &values[3], &values[4], &values[5]);
          if (rc)
            goto leave;
        }
    }

  /* Guess required fields if a curve parameter has not been given.
     FIXME: This is a crude hacks.  We need to fix that.  */
  if (!curvename)
    {
      model = ((flags & PUBKEY_FLAG_EDDSA)
               ? MPI_EC_EDWARDS
               : MPI_EC_WEIERSTRASS);
      dialect = ((flags & PUBKEY_FLAG_EDDSA)
                 ? ECC_DIALECT_ED25519
                 : ECC_DIALECT_STANDARD);
      if (!values[5])
	values[5] = mpi_const (MPI_C_ONE);
    }

  /* Check that all parameters are known and normalize all MPIs (that
     should not be required but we use an internal function later and
     thus we better make 100% sure that they are normalized). */
  for (idx = 0; idx < N_COMPONENTS; idx++)
    if (!values[idx])
      {
        rc = GPG_ERR_NO_OBJ;
        goto leave;
      }
    else
      _gcry_mpi_normalize (values[idx]);

  /* Uncompress the public key with the exception of EdDSA where
     compression is the default and we thus compute the keygrip using
     the compressed version.  Because we don't support any non-eddsa
     compression, the only thing we need to do is to compress
     EdDSA.  */
  if ((flags & PUBKEY_FLAG_DJB_TWEAK))
    {
      rc = _gcry_ecc_eddsa_ensure_compact (values[6], 256);
      if (rc)
        goto leave;
    }

  /* Hash them all.  */
  for (idx = 0; idx < N_COMPONENTS; idx++)
    {
      char buf[30];

      if (idx == 5)
        continue;               /* Skip cofactor. */

      if (mpi_is_opaque (values[idx]))
        {
          const unsigned char *raw;
          unsigned int n;

          raw = mpi_get_opaque (values[idx], &n);
          n = (n + 7)/8;
          snprintf (buf, sizeof buf, "(1:%c%u:", names[idx], n);
          _gcry_md_write (md, buf, strlen (buf));
          _gcry_md_write (md, raw, n);
          _gcry_md_write (md, ")", 1);
        }
      else
        {
          unsigned char *rawmpi;
          unsigned int rawmpilen;

          rawmpi = _gcry_mpi_get_buffer (values[idx], 0, &rawmpilen, NULL);
          if (!rawmpi)
            {
              rc = gpg_err_code_from_syserror ();
              goto leave;
            }
          snprintf (buf, sizeof buf, "(1:%c%u:", names[idx], rawmpilen);
          _gcry_md_write (md, buf, strlen (buf));
          _gcry_md_write (md, rawmpi, rawmpilen);
          _gcry_md_write (md, ")", 1);
          xfree (rawmpi);
        }
    }

 leave:
  xfree (curvename);
  sexp_release (l1);
  for (idx = 0; idx < N_COMPONENTS; idx++)
    _gcry_mpi_release (values[idx]);

  return rc;
#undef N_COMPONENTS
}