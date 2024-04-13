static uint32_t *rpc_add_credentials(uint32_t *p)
{
	/*
	 * *BSD refuses AUTH_NONE, so use AUTH_UNIX. An empty hostname is OK for
	 * both Linux and *BSD.
	 */

	/* Provide an AUTH_UNIX credential.  */
	*p++ = hton32(1);		/* AUTH_UNIX */
	*p++ = hton32(20);		/* auth length: 20 + strlen(hostname) */
	*p++ = hton32(0);		/* stamp */
	*p++ = hton32(0);		/* hostname string length */
	/* memcpy(p, "", 0); p += 0; <- empty host name */

	*p++ = 0;			/* uid */
	*p++ = 0;			/* gid */
	*p++ = 0;			/* auxiliary gid list */

	/* Provide an AUTH_NONE verifier.  */
	*p++ = 0;			/* AUTH_NONE */
	*p++ = 0;			/* auth length */

	return p;
}
