static Bool txtin_process_event(GF_Filter *filter, const GF_FilterEvent *evt)
{
	GF_TXTIn *ctx = gf_filter_get_udta(filter);
	switch (evt->base.type) {
	case GF_FEVT_PLAY:
		if (ctx->playstate==1) return GF_TRUE;
		ctx->playstate = 1;
		if ((ctx->start_range < 0.1) && (evt->play.start_range<0.1)) return GF_TRUE;
		ctx->start_range = evt->play.start_range;
		ctx->seek_state = 1;
		//cancel play event, we work with full file
		return GF_TRUE;

	case GF_FEVT_STOP:
		ctx->playstate = 2;
		//cancel play event, we work with full file
		return GF_TRUE;
	default:
		return GF_FALSE;
	}
	return GF_FALSE;
}