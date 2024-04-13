const char *charset_name(struct smb_iconv_handle *ic, charset_t ch)
{
	switch (ch) {
	case CH_UTF16: return "UTF-16LE";
	case CH_UNIX: return ic->unix_charset;
	case CH_DOS: return ic->dos_charset;
	case CH_UTF8: return "UTF8";
	case CH_UTF16BE: return "UTF-16BE";
	case CH_UTF16MUNGED: return "UTF16_MUNGED";
	default:
	return "ASCII";
	}
}
