static int wait_for_data(SSL *ssl, int ret, int timeout)
{
  int err;
  int retval = 1;

  err = SSL_get_error(ssl, ret);
	
  switch(err)
  {
    case SSL_ERROR_WANT_READ:
    case SSL_ERROR_WANT_WRITE:
      ret = sock_read_ready(ssl, timeout*1000);
			
      if (ret == -1) {
        retval = 2;
      }
				
      break;
    default:
      retval = 3;
      long verify_res = SSL_get_verify_result(ssl);
      if (verify_res != X509_V_OK)
        rfbClientLog("Could not verify server certificate: %s.\n",
                     X509_verify_cert_error_string(verify_res));
      break;
   }
	
  ERR_clear_error();
				
  return retval;
}