int ssl3_pending(const SSL *s)
	{
	if (s->rstate == SSL_ST_READ_BODY)
		return 0;
	
	return (s->s3->rrec.type == SSL3_RT_APPLICATION_DATA) ? s->s3->rrec.length : 0;
	}
