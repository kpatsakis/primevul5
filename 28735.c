const SSL_CIPHER *ssl3_get_cipher_by_char(const unsigned char *p)
	{
	SSL_CIPHER c;
	const SSL_CIPHER *cp;
	unsigned long id;

	id=0x03000000L|((unsigned long)p[0]<<8L)|(unsigned long)p[1];
	c.id=id;
	cp = OBJ_bsearch_ssl_cipher_id(&c, ssl3_ciphers, SSL3_NUM_CIPHERS);
#ifdef DEBUG_PRINT_UNKNOWN_CIPHERSUITES
if (cp == NULL) fprintf(stderr, "Unknown cipher ID %x\n", (p[0] << 8) | p[1]);
#endif
	if (cp == NULL || cp->valid == 0)
		return NULL;
	else
		return cp;
	}
