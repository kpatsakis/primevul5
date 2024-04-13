static GF_Err gf_webvtt_import_report(void *user, GF_Err e, char *message, const char *line)
{
	GF_LOG(e ? GF_LOG_WARNING : GF_LOG_INFO, GF_LOG_AUTHOR, ("[TXTIn] WebVTT line %s: %s\n", line, message) );
	return e;
}