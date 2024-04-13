void ssl3_clear(SSL *s)
	{
	unsigned char *rp,*wp;
	size_t rlen, wlen;
	int init_extra;

#ifdef TLSEXT_TYPE_opaque_prf_input
	if (s->s3->client_opaque_prf_input != NULL)
		OPENSSL_free(s->s3->client_opaque_prf_input);
	s->s3->client_opaque_prf_input = NULL;
	if (s->s3->server_opaque_prf_input != NULL)
		OPENSSL_free(s->s3->server_opaque_prf_input);
	s->s3->server_opaque_prf_input = NULL;
#endif

	ssl3_cleanup_key_block(s);
	if (s->s3->tmp.ca_names != NULL)
		sk_X509_NAME_pop_free(s->s3->tmp.ca_names,X509_NAME_free);

	if (s->s3->rrec.comp != NULL)
		{
		OPENSSL_free(s->s3->rrec.comp);
		s->s3->rrec.comp=NULL;
		}
#ifndef OPENSSL_NO_DH
	if (s->s3->tmp.dh != NULL)
		{
		DH_free(s->s3->tmp.dh);
		s->s3->tmp.dh = NULL;
		}
#endif
#ifndef OPENSSL_NO_ECDH
	if (s->s3->tmp.ecdh != NULL)
		{
		EC_KEY_free(s->s3->tmp.ecdh);
		s->s3->tmp.ecdh = NULL;
		}
#endif
#ifndef OPENSSL_NO_TLSEXT
#ifndef OPENSSL_NO_EC
	s->s3->is_probably_safari = 0;
#endif /* !OPENSSL_NO_EC */
#endif /* !OPENSSL_NO_TLSEXT */

	rp = s->s3->rbuf.buf;
	wp = s->s3->wbuf.buf;
	rlen = s->s3->rbuf.len;
 	wlen = s->s3->wbuf.len;
	init_extra = s->s3->init_extra;
	if (s->s3->handshake_buffer) {
		BIO_free(s->s3->handshake_buffer);
		s->s3->handshake_buffer = NULL;
	}
	if (s->s3->handshake_dgst) {
		ssl3_free_digest_list(s);
	}	
	memset(s->s3,0,sizeof *s->s3);
	s->s3->rbuf.buf = rp;
	s->s3->wbuf.buf = wp;
	s->s3->rbuf.len = rlen;
 	s->s3->wbuf.len = wlen;
	s->s3->init_extra = init_extra;

	ssl_free_wbio_buffer(s);

	s->packet_length=0;
	s->s3->renegotiate=0;
	s->s3->total_renegotiations=0;
	s->s3->num_renegotiations=0;
	s->s3->in_read_app_data=0;
	s->version=SSL3_VERSION;

#if !defined(OPENSSL_NO_TLSEXT) && !defined(OPENSSL_NO_NEXTPROTONEG)
	if (s->next_proto_negotiated)
		{
		OPENSSL_free(s->next_proto_negotiated);
		s->next_proto_negotiated = NULL;
		s->next_proto_negotiated_len = 0;
		}
#endif
	}
