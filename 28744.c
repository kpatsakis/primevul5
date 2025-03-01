int ssl3_shutdown(SSL *s)
	{
	int ret;

	/* Don't do anything much if we have not done the handshake or
	 * we don't want to send messages :-) */
	if ((s->quiet_shutdown) || (s->state == SSL_ST_BEFORE))
		{
		s->shutdown=(SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN);
		return(1);
		}

	if (!(s->shutdown & SSL_SENT_SHUTDOWN))
		{
		s->shutdown|=SSL_SENT_SHUTDOWN;
#if 1
		ssl3_send_alert(s,SSL3_AL_WARNING,SSL_AD_CLOSE_NOTIFY);
#endif
		/* our shutdown alert has been sent now, and if it still needs
	 	 * to be written, s->s3->alert_dispatch will be true */
	 	if (s->s3->alert_dispatch)
	 		return(-1);	/* return WANT_WRITE */
		}
	else if (s->s3->alert_dispatch)
		{
		/* resend it if not sent */
#if 1
		ret=s->method->ssl_dispatch_alert(s);
		if(ret == -1)
			{
			/* we only get to return -1 here the 2nd/Nth
			 * invocation, we must  have already signalled
			 * return 0 upon a previous invoation,
			 * return WANT_WRITE */
			return(ret);
			}
#endif
		}
	else if (!(s->shutdown & SSL_RECEIVED_SHUTDOWN))
		{
		/* If we are waiting for a close from our peer, we are closed */
		s->method->ssl_read_bytes(s,0,NULL,0,0);
		if(!(s->shutdown & SSL_RECEIVED_SHUTDOWN))
			{
			return(-1);	/* return WANT_READ */
			}
		}

	if ((s->shutdown == (SSL_SENT_SHUTDOWN|SSL_RECEIVED_SHUTDOWN)) &&
		!s->s3->alert_dispatch)
		return(1);
	else
		return(0);
	}
