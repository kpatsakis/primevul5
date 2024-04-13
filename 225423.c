static php_iconv_err_t php_iconv_stream_filter_ctor(php_iconv_stream_filter *self,
		const char *to_charset, size_t to_charset_len,
		const char *from_charset, size_t from_charset_len, int persistent)
{
	if (NULL == (self->to_charset = pemalloc(to_charset_len + 1, persistent))) {
		return PHP_ICONV_ERR_ALLOC;
	}
	self->to_charset_len = to_charset_len;
	if (NULL == (self->from_charset = pemalloc(from_charset_len + 1, persistent))) {
		pefree(self->to_charset, persistent);
		return PHP_ICONV_ERR_ALLOC;
	}
	self->from_charset_len = from_charset_len;

	memcpy(self->to_charset, to_charset, to_charset_len);
	self->to_charset[to_charset_len] = '\0';
	memcpy(self->from_charset, from_charset, from_charset_len);
	self->from_charset[from_charset_len] = '\0';

	if ((iconv_t)-1 == (self->cd = iconv_open(self->to_charset, self->from_charset))) {
		pefree(self->from_charset, persistent);
		pefree(self->to_charset, persistent);
		return PHP_ICONV_ERR_UNKNOWN;
	}
	self->persistent = persistent;
	self->stub_len = 0;
	return PHP_ICONV_ERR_SUCCESS;
}