static GF_Err ttml_push_interval(GF_TXTIn *ctx, s64 begin, s64 end, TTMLInterval **out_interval)
{
	u32 i;
	TTMLInterval *interval;
	if (begin==-1) return GF_OK;
	if (end==-1) return GF_OK;

	if (end < begin) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_PARSER, ("[TTML EBU-TTD] invalid timings: \"begin\"="LLD" , \"end\"="LLD". Abort.\n", begin, end));
		return GF_NON_COMPLIANT_BITSTREAM;
	}

	interval = NULL;
	for (i=0; i<gf_list_count(ctx->intervals); i++) {
		interval = gf_list_get(ctx->intervals, i);

		//generate a single sample for the input, merge interval
		if (ctx->ttml_dur>=0) {
			if (interval->begin > begin) interval->begin = begin;
			if (interval->end < end) interval->end = end;
			*out_interval = interval;
			return GF_OK;
		}
		//contained, do nothing
		if ((begin>=interval->begin) && (end<=interval->end)) {
			*out_interval = interval;
			return GF_OK;
		}
		//not overlapping
		if ((end < interval->begin) || (begin > interval->end))
			continue;

		//new interval starts before current and end after, remove current and push extended interval
		if ((begin < interval->end) && (end > interval->end)) {
			if (begin>interval->begin)
				begin = interval->begin;
			gf_list_rem(ctx->intervals, i);
			gf_free(interval);
			return ttml_push_interval(ctx, begin, end, out_interval);
		}
		//new interval starts before current and end before, remove current and push rewinded
		if ((begin < interval->end) && (end <= interval->end)) {
			end = interval->end;
			if (begin>interval->begin)
				begin = interval->begin;
			gf_list_rem(ctx->intervals, i);
			gf_free(interval);
			return ttml_push_interval(ctx, begin, end, out_interval);
		}
	}
	//need a new interval
	GF_SAFEALLOC(interval, TTMLInterval);
	interval->begin = begin;
	interval->end = end;
	*out_interval = interval;

	for (i=0; i<gf_list_count(ctx->intervals); i++) {
		TTMLInterval *an_interval = gf_list_get(ctx->intervals, i);
		if (an_interval->begin > interval->begin) {
			return gf_list_insert(ctx->intervals, interval, i);
		}
	}
	return gf_list_add(ctx->intervals, interval);
}