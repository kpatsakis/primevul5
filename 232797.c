int _san_othername_to_virtual(const char *oid, size_t size)
{
	if (oid) {
		if ((unsigned) size == (sizeof(XMPP_OID)-1)
		    && memcmp(oid, XMPP_OID, sizeof(XMPP_OID)-1) == 0)
			return GNUTLS_SAN_OTHERNAME_XMPP;
	}

	return GNUTLS_SAN_OTHERNAME;
} 