mac_init (digest_hd_st* td, gnutls_mac_algorithm_t mac, opaque * secret, int secret_size,
	  int ver)
{
int ret =  0;

  if (mac == GNUTLS_MAC_NULL)
    {
      gnutls_assert();
      return GNUTLS_E_HASH_FAILED;
    }

  if (ver == GNUTLS_SSL3)
    {				/* SSL 3.0 */
      ret = _gnutls_mac_init_ssl3 (td, mac, secret, secret_size);
    }
  else
    {				/* TLS 1.x */
      ret = _gnutls_hmac_init (td, mac, secret, secret_size);
    }

  return ret;
}