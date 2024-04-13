static void con_flush(struct vc_data *vc, unsigned long draw_from,
		unsigned long draw_to, int *draw_x)
{
	if (*draw_x < 0)
		return;

	vc->vc_sw->con_putcs(vc, (u16 *)draw_from,
			(u16 *)draw_to - (u16 *)draw_from, vc->vc_y, *draw_x);
	*draw_x = -1;
}