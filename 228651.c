_gnutls_compressed2ciphertext (gnutls_session_t session,
			       opaque * cipher_data, int cipher_size,
			       gnutls_datum_t compressed,
			       content_type_t _type, int random_pad)
{
  uint8_t MAC[MAX_HASH_SIZE];
  uint16_t c_length;
  uint8_t pad;
  int length, ret;
  digest_hd_st td;
  uint8_t type = _type;
  uint8_t major, minor;
  int hash_size =
    _gnutls_hash_get_algo_len (session->security_parameters.
			       write_mac_algorithm);
  gnutls_protocol_t ver;
  int blocksize =
    _gnutls_cipher_get_block_size (session->security_parameters.
				   write_bulk_cipher_algorithm);
  cipher_type_t block_algo =
    _gnutls_cipher_is_block (session->security_parameters.
			     write_bulk_cipher_algorithm);
  opaque *data_ptr;


  ver = gnutls_protocol_get_version (session);
  minor = _gnutls_version_get_minor (ver);
  major = _gnutls_version_get_major (ver);


  /* Initialize MAC */
  ret = mac_init (&td, session->security_parameters.write_mac_algorithm,
		 session->connection_state.write_mac_secret.data,
		 session->connection_state.write_mac_secret.size, ver);

  if (ret < 0 && session->security_parameters.write_mac_algorithm != GNUTLS_MAC_NULL)
    {
      gnutls_assert ();
      return ret;
    }

  c_length = _gnutls_conv_uint16 (compressed.size);

  if (session->security_parameters.write_mac_algorithm != GNUTLS_MAC_NULL)
    {				/* actually when the algorithm in not the NULL one */
      _gnutls_hmac (&td,
		    UINT64DATA (session->connection_state.
				write_sequence_number), 8);

      _gnutls_hmac (&td, &type, 1);
      if (ver >= GNUTLS_TLS1)
	{			/* TLS 1.0 or higher */
	  _gnutls_hmac (&td, &major, 1);
	  _gnutls_hmac (&td, &minor, 1);
	}
      _gnutls_hmac (&td, &c_length, 2);
      _gnutls_hmac (&td, compressed.data, compressed.size);
      mac_deinit (&td, MAC, ver);
    }


  /* Calculate the encrypted length (padding etc.)
   */
  length =
    calc_enc_length (session, compressed.size, hash_size, &pad,
		     random_pad, block_algo, blocksize);
  if (length < 0)
    {
      gnutls_assert ();
      return length;
    }

  /* copy the encrypted data to cipher_data.
   */
  if (cipher_size < length)
    {
      gnutls_assert ();
      return GNUTLS_E_MEMORY_ERROR;
    }

  data_ptr = cipher_data;
  if (block_algo == CIPHER_BLOCK &&
      session->security_parameters.version >= GNUTLS_TLS1_1)
    {
      /* copy the random IV.
       */
      ret = _gnutls_rnd (RND_NONCE, data_ptr, blocksize);
      if (ret < 0)
	{
	  gnutls_assert ();
	  return ret;
	}

      data_ptr += blocksize;
    }

  memcpy (data_ptr, compressed.data, compressed.size);
  data_ptr += compressed.size;

  if (hash_size > 0)
    {
      memcpy (data_ptr, MAC, hash_size);
      data_ptr += hash_size;
    }
  if (block_algo == CIPHER_BLOCK && pad > 0)
    {
      memset (data_ptr, pad - 1, pad);
    }


  /* Actual encryption (inplace).
   */
  ret = _gnutls_cipher_encrypt (&session->connection_state.
				write_cipher_state, cipher_data, length);
  if (ret < 0)
    {
      gnutls_assert ();
      return ret;
    }

  return length;
}