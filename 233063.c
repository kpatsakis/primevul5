void FreeTLS(rfbClient* client)
{
  int i;

  if (mutex_buf != NULL) {
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);

    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_id_callback(NULL);

    for (i = 0; i < CRYPTO_num_locks(); i++)
      MUTEX_FREE(mutex_buf[i]);
    free(mutex_buf);
    mutex_buf = NULL;
  }

  SSL_free(client->tlsSession);
}