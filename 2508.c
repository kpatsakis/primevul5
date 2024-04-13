static GF_Err gf_text_process_swf(GF_Filter *filter, GF_TXTIn *ctx)
{
	GF_LOG(GF_LOG_WARNING, GF_LOG_PARSER, ("Warning: GPAC was compiled without SWF import support, can't import file.\n"));
	return GF_NOT_SUPPORTED;
}