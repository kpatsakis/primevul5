static void vc_uniscr_putc(struct vc_data *vc, char32_t uc)
{
	struct uni_screen *uniscr = get_vc_uniscr(vc);

	if (uniscr)
		uniscr->lines[vc->vc_y][vc->vc_x] = uc;
}