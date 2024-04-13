u32 screen_glyph_unicode(struct vc_data *vc, int n)
{
	struct uni_screen *uniscr = get_vc_uniscr(vc);

	if (uniscr)
		return uniscr->lines[n / vc->vc_cols][n % vc->vc_cols];
	return inverse_translate(vc, screen_glyph(vc, n * 2), 1);
}