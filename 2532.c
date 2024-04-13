GF_Err txtin_initialize(GF_Filter *filter)
{
	char data[1];
	GF_TXTIn *ctx = gf_filter_get_udta(filter);
	ctx->bs_w = gf_bs_new(data, 1, GF_BITSTREAM_WRITE);

	return GF_OK;
}