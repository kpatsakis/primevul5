_PUBLIC_ char *strupper_talloc_n(TALLOC_CTX *ctx, const char *src, size_t n)
{
	struct smb_iconv_handle *iconv_handle = get_iconv_handle();
	return strupper_talloc_n_handle(iconv_handle, ctx, src, n);
}
