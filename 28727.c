long ssl3_callback_ctrl(SSL *s, int cmd, void (*fp)(void))
	{
	int ret=0;

#if !defined(OPENSSL_NO_DSA) || !defined(OPENSSL_NO_RSA)
	if (
#ifndef OPENSSL_NO_RSA
	    cmd == SSL_CTRL_SET_TMP_RSA_CB ||
#endif
#ifndef OPENSSL_NO_DSA
	    cmd == SSL_CTRL_SET_TMP_DH_CB ||
#endif
		0)
		{
		if (!ssl_cert_inst(&s->cert))
			{
			SSLerr(SSL_F_SSL3_CALLBACK_CTRL, ERR_R_MALLOC_FAILURE);
			return(0);
			}
		}
#endif

	switch (cmd)
		{
#ifndef OPENSSL_NO_RSA
	case SSL_CTRL_SET_TMP_RSA_CB:
		{
		s->cert->rsa_tmp_cb = (RSA *(*)(SSL *, int, int))fp;
		}
		break;
#endif
#ifndef OPENSSL_NO_DH
	case SSL_CTRL_SET_TMP_DH_CB:
		{
		s->cert->dh_tmp_cb = (DH *(*)(SSL *, int, int))fp;
		}
		break;
#endif
#ifndef OPENSSL_NO_ECDH
	case SSL_CTRL_SET_TMP_ECDH_CB:
		{
		s->cert->ecdh_tmp_cb = (EC_KEY *(*)(SSL *, int, int))fp;
		}
		break;
#endif
#ifndef OPENSSL_NO_TLSEXT
	case SSL_CTRL_SET_TLSEXT_DEBUG_CB:
		s->tlsext_debug_cb=(void (*)(SSL *,int ,int,
					unsigned char *, int, void *))fp;
		break;
#endif
	default:
		break;
		}
	return(ret);
	}
