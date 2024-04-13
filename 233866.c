ms_escher_opt_end (GString *buf, gsize marker)
{
	/* Length does not include header.  */
	gsize len = buf->len - marker - 8;
	GSF_LE_SET_GUINT32 (buf->str + marker + 4, len);
}