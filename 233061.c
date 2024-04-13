InitializeTLS(void)
{
  int i;

  if (rfbTLSInitialized) return TRUE;

  mutex_buf = malloc(CRYPTO_num_locks() * sizeof(MUTEX_TYPE));
  if (mutex_buf == NULL) {
    rfbClientLog("Failed to initialized OpenSSL: memory.\n");
    return (-1);
  }

  for (i = 0; i < CRYPTO_num_locks(); i++)
    MUTEX_INIT(mutex_buf[i]);

  CRYPTO_set_locking_callback(locking_function);
  CRYPTO_set_id_callback(id_function);
  CRYPTO_set_dynlock_create_callback(dyn_create_function);
  CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
  CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
  SSL_load_error_strings();
  SSLeay_add_ssl_algorithms();
  RAND_load_file("/dev/urandom", 1024);

  rfbClientLog("OpenSSL version %s initialized.\n", SSLeay_version(SSLEAY_VERSION));
  rfbTLSInitialized = TRUE;
  return TRUE;
}