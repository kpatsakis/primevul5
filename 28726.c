static char * MS_CALLBACK srp_password_from_info_cb(SSL *s, void *arg)
	{
	return BUF_strdup(s->srp_ctx.info) ;
	}
