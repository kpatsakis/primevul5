static void vc_uniscr_clear_line(struct vc_data *vc, unsigned int x,
				 unsigned int nr)
{
	struct uni_screen *uniscr = get_vc_uniscr(vc);

	if (uniscr) {
		char32_t *ln = uniscr->lines[vc->vc_y];

		memset32(&ln[x], ' ', nr);
	}
}