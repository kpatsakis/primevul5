static void ttxt_dom_progress(void *cbk, u64 cur_samp, u64 count)
{
	GF_TXTIn *ctx = (GF_TXTIn *)cbk;
	ctx->end = count;
}