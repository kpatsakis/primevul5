void dcn20_dsc_destroy(struct display_stream_compressor **dsc)
{
	kfree(container_of(*dsc, struct dcn20_dsc, base));
	*dsc = NULL;
}