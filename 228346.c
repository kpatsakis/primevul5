PHP_FUNCTION(quoted_printable_decode)
{
	char *arg1, *str_in, *str_out;
	int arg1_len, i = 0, j = 0, k;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg1, &arg1_len) == FAILURE) {
		return;
	}
    
	if (arg1_len == 0) {
		/* shortcut */
		RETURN_EMPTY_STRING();
	}

	str_in = arg1;
	str_out = emalloc(arg1_len + 1);
	while (str_in[i]) {
		switch (str_in[i]) {
		case '=':
			if (str_in[i + 1] && str_in[i + 2] && 
				isxdigit((int) str_in[i + 1]) && 
				isxdigit((int) str_in[i + 2]))
			{
				str_out[j++] = (php_hex2int((int) str_in[i + 1]) << 4) 
						+ php_hex2int((int) str_in[i + 2]);
				i += 3;
			} else  /* check for soft line break according to RFC 2045*/ {
				k = 1;
				while (str_in[i + k] && ((str_in[i + k] == 32) || (str_in[i + k] == 9))) {
					/* Possibly, skip spaces/tabs at the end of line */
					k++;
				}
				if (!str_in[i + k]) {
					/* End of line reached */
					i += k;
				}
				else if ((str_in[i + k] == 13) && (str_in[i + k + 1] == 10)) {
					/* CRLF */
					i += k + 2;
				}
				else if ((str_in[i + k] == 13) || (str_in[i + k] == 10)) {
					/* CR or LF */
					i += k + 1;
				}
				else {
					str_out[j++] = str_in[i++];
				}
			}
			break;
		default:
			str_out[j++] = str_in[i++];
		}
	}
	str_out[j] = '\0';
    
	RETVAL_STRINGL(str_out, j, 0);
}