_PUBLIC_ struct smb_iconv_handle *smb_iconv_handle_reinit(TALLOC_CTX *mem_ctx,
								    const char *dos_charset,
								    const char *unix_charset,
								    bool use_builtin_handlers,
								    struct smb_iconv_handle *old_ic)
{
	struct smb_iconv_handle *ret;

	if (old_ic != NULL) {
		ret = old_ic;
		close_iconv_handle(ret);
		talloc_free(ret->child_ctx);
		ZERO_STRUCTP(ret);
	} else {
		ret = talloc_zero(mem_ctx, struct smb_iconv_handle);
	}
	if (ret == NULL) {
		return NULL;
	}

	/* we use a child context to allow us to free all ptrs without
	   freeing the structure itself */
	ret->child_ctx = talloc_new(ret);
	if (ret->child_ctx == NULL) {
		return NULL;
	}

	talloc_set_destructor(ret, close_iconv_handle);

	if (strcasecmp(dos_charset, "UTF8") == 0 || strcasecmp(dos_charset, "UTF-8") == 0) {
		DEBUG(0,("ERROR: invalid DOS charset: 'dos charset' must not be UTF8, using (default value) CP850 instead\n"));
		dos_charset = "CP850";
	}

	ret->dos_charset = talloc_strdup(ret->child_ctx, dos_charset);
	ret->unix_charset = talloc_strdup(ret->child_ctx, unix_charset);
	ret->use_builtin_handlers = use_builtin_handlers;

	return ret;
}
