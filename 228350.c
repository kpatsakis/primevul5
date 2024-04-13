PHPAPI unsigned char *php_quot_print_decode(const unsigned char *str, size_t length, size_t *ret_length, int replace_us_by_ws) /* {{{ */
{
	register unsigned int i;
	register unsigned const char *p1;
	register unsigned char *p2;
	register unsigned int h_nbl, l_nbl;

	size_t decoded_len, buf_size;
	unsigned char *retval;

	static unsigned int hexval_tbl[256] = {
		64, 64, 64, 64, 64, 64, 64, 64, 64, 32, 16, 64, 64, 16, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		32, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 64, 64, 64, 64, 64, 64,
		64, 10, 11, 12, 13, 14, 15, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 10, 11, 12, 13, 14, 15, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
		64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
	};

	if (replace_us_by_ws) {
		replace_us_by_ws = '_';
	}

	i = length, p1 = str; buf_size = length;

	while (i > 1 && *p1 != '\0') {
		if (*p1 == '=') {
			buf_size -= 2;
			p1++;
			i--;
		}
		p1++;
		i--;
	}

	retval = emalloc(buf_size + 1);
	i = length; p1 = str; p2 = retval;
	decoded_len = 0;

	while (i > 0 && *p1 != '\0') {
		if (*p1 == '=') {
			i--, p1++;
			if (i == 0 || *p1 == '\0') {
				break;
			}
			h_nbl = hexval_tbl[*p1];
			if (h_nbl < 16) {
				/* next char should be a hexadecimal digit */
				if ((--i) == 0 || (l_nbl = hexval_tbl[*(++p1)]) >= 16) {
					efree(retval);
					return NULL;
				}
				*(p2++) = (h_nbl << 4) | l_nbl, decoded_len++;
				i--, p1++;
			} else if (h_nbl < 64) {
				/* soft line break */
				while (h_nbl == 32) {
					if (--i == 0 || (h_nbl = hexval_tbl[*(++p1)]) == 64) {
						efree(retval);
						return NULL;
					}
				}
				if (p1[0] == '\r' && i >= 2 && p1[1] == '\n') {
					i--, p1++;
				}
				i--, p1++;
			} else {
				efree(retval);
				return NULL;
			}
		} else {
			*(p2++) = (replace_us_by_ws == *p1 ? '\x20': *p1);
			i--, p1++, decoded_len++;
		}
	}

	*p2 = '\0';
	*ret_length = decoded_len;
	return retval;
}