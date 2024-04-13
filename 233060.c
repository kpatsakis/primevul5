ssl_errno (SSL *ssl, int ret)
{
	switch (SSL_get_error (ssl, ret)) {
	case SSL_ERROR_NONE:
		return 0;
	case SSL_ERROR_ZERO_RETURN:
		/* this one does not map well at all */
		//d(printf ("ssl_errno: SSL_ERROR_ZERO_RETURN\n"));
		return EINVAL;
	case SSL_ERROR_WANT_READ:   /* non-fatal; retry */
	case SSL_ERROR_WANT_WRITE:  /* non-fatal; retry */
		//d(printf ("ssl_errno: SSL_ERROR_WANT_[READ,WRITE]\n"));
		return EAGAIN;
	case SSL_ERROR_SYSCALL:
		//d(printf ("ssl_errno: SSL_ERROR_SYSCALL\n"));
		return EINTR;
	case SSL_ERROR_SSL:
		//d(printf ("ssl_errno: SSL_ERROR_SSL  <-- very useful error...riiiiight\n"));
		return EINTR;
	default:
		//d(printf ("ssl_errno: default error\n"));
		return EINTR;
	}
}