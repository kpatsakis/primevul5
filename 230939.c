static void vc_uniscr_insert(struct vc_data *vc, unsigned int nr)
{
	struct uni_screen *uniscr = get_vc_uniscr(vc);

	if (uniscr) {
		char32_t *ln = uniscr->lines[vc->vc_y];
		unsigned int x = vc->vc_x, cols = vc->vc_cols;

		memmove(&ln[x + nr], &ln[x], (cols - x - nr) * sizeof(*ln));
		memset32(&ln[x], ' ', nr);
	}
}