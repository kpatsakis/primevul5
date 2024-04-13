static GF_Err ttml_rewrite_timestamp(GF_TXTIn *ctx, s64 ttml_zero, GF_XMLAttribute *att, s64 *value, Bool *drop)
{
	u64 v;
	char szTS[21];
	u32 h, m, s, ms;
	*value = ttml_get_timestamp(ctx, att->value);
	if (!ttml_zero)
		return GF_OK;

	if (*value < ttml_zero) {
		*drop = GF_TRUE;
		return GF_OK;
	}

	*value -= ttml_zero;
	v = (u64) (*value / 1000);
	h = (u32) (v / 3600);
	m = (u32) (v - h*60) / 60;
	s = (u32) (v - h*3600 - m*60);
	ms = (*value) % 1000;

	snprintf(szTS, 20, "%02d:%02d:%02d.%03d", h, m, s, ms);
	szTS[20] = 0;
	gf_free(att->value);
	att->value = gf_strdup(szTS);
	return GF_OK;
}