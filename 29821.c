struct smb_iconv_handle *get_iconv_handle(void)
{
	if (global_iconv_handle == NULL)
		global_iconv_handle = smb_iconv_handle_reinit(talloc_autofree_context(),
							      "ASCII", "UTF-8", true, NULL);
	return global_iconv_handle;
}
