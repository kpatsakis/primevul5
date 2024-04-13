struct smb_iconv_handle *get_iconv_testing_handle(TALLOC_CTX *mem_ctx, 
						  const char *dos_charset, 
						  const char *unix_charset,
						  bool use_builtin_handlers)
{
	return smb_iconv_handle_reinit(mem_ctx,
				       dos_charset, unix_charset, use_builtin_handlers, NULL);
}
