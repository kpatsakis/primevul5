mac_deinit (digest_hd_st *td, opaque * res, int ver)
{
  if (ver == GNUTLS_SSL3)
    {				/* SSL 3.0 */
      _gnutls_mac_deinit_ssl3 (td, res);
    }
  else
    {
      _gnutls_hmac_deinit (td, res);
    }
}