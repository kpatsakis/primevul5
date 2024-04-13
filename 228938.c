get_cmdline_cipher_suite_priv_data(char * arg, uint8_t * buf)
{
	int i, ret = 0;

	if (strlen(arg) != 15)
	{
		lprintf(LOG_ERR, "Invalid privilege specification length: %d",
			strlen(arg));
		return -1;
	}

	/*
	 * The first byte is reserved (0).  The rest of the buffer is setup
	 * so that each nibble holds the maximum privilege level available for
	 * that cipher suite number.  The number of nibbles (15) matches the number
	 * of fixed cipher suite IDs.  This command documentation mentions 16 IDs
	 * but table 22-19 shows that there are only 15 (0-14).
	 *
	 * data 1 - reserved
	 * data 2 - maximum priv level for first (LSN) and second (MSN) ciphers
	 * data 3 - maximum priv level for third (LSN) and fourth (MSN) ciphers
	 * data 9 - maximum priv level for 15th (LSN) cipher.
	 */
	memset(buf, 0, 9);
	for (i = 0; i < 15; ++i)
	{
		unsigned char priv_level = IPMI_SESSION_PRIV_ADMIN;

		switch (arg[i])
		{
		case 'X':
			priv_level = IPMI_SESSION_PRIV_UNSPECIFIED; /* 0 */
			break;
		case 'c':
			priv_level = IPMI_SESSION_PRIV_CALLBACK;    /* 1 */
			break;
		case 'u':
			priv_level = IPMI_SESSION_PRIV_USER;        /* 2 */
			break;
		case 'o':
			priv_level = IPMI_SESSION_PRIV_OPERATOR;    /* 3 */
			break;
		case 'a':
			priv_level = IPMI_SESSION_PRIV_ADMIN;       /* 4 */
			break;
		case 'O':
			priv_level = IPMI_SESSION_PRIV_OEM;         /* 5 */
			break;
		default:
			lprintf(LOG_ERR, "Invalid privilege specification char: %c",
				arg[i]);
			ret = -1;
			break;
		}

		if (ret != 0)
			break;
		else
		{
			if ((i + 1) % 2)
			{
				// Odd number cipher suites will be in the LSN
				buf[1 + (i / 2)] += priv_level;
			}
			else
			{
				// Even number cipher suites will be in the MSN
				buf[1 + (i / 2)] += (priv_level << 4);
			}
		}
	}

	return ret;
}