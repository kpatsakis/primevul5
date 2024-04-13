static void csi_M(struct vc_data *vc, unsigned int nr)
{
	if (nr > vc->vc_rows - vc->vc_y)
		nr = vc->vc_rows - vc->vc_y;
	else if (!nr)
		nr=1;
	con_scroll(vc, vc->vc_y, vc->vc_bottom, SM_UP, nr);
	vc->vc_need_wrap = 0;
}