PHPAPI unsigned char *php_quot_print_encode(const unsigned char *str, size_t length, size_t *ret_length) /* {{{ */
{
	unsigned long lp = 0;
	unsigned char c, *ret, *d;
	char *hex = "0123456789ABCDEF";

	ret = safe_emalloc(1, 3 * length + 3 * (((3 * length)/PHP_QPRINT_MAXL) + 1), 0);
	d = ret;

	while (length--) {
		if (((c = *str++) == '\015') && (*str == '\012') && length > 0) {
			*d++ = '\015';
			*d++ = *str++;
			length--;
			lp = 0;
		} else {
			if (iscntrl (c) || (c == 0x7f) || (c & 0x80) || (c == '=') || ((c == ' ') && (*str == '\015'))) {
				if ((((lp+= 3) > PHP_QPRINT_MAXL) && (c <= 0x7f)) 
            || ((c > 0x7f) && (c <= 0xdf) && ((lp + 3) > PHP_QPRINT_MAXL)) 
            || ((c > 0xdf) && (c <= 0xef) && ((lp + 6) > PHP_QPRINT_MAXL)) 
            || ((c > 0xef) && (c <= 0xf4) && ((lp + 9) > PHP_QPRINT_MAXL))) {
					*d++ = '=';
					*d++ = '\015';
					*d++ = '\012';
					lp = 3;
				}
				*d++ = '=';
				*d++ = hex[c >> 4];
				*d++ = hex[c & 0xf];
			} else {
				if ((++lp) > PHP_QPRINT_MAXL) {
					*d++ = '=';
					*d++ = '\015';
					*d++ = '\012';
					lp = 1;
				}
				*d++ = c;
			}
		}
	}
	*d = '\0';
	*ret_length = d - ret;

	ret = erealloc(ret, *ret_length + 1);
	return ret;
}