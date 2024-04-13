static void csi_P(struct vc_data *vc, unsigned int nr)
{
	if (nr > vc->vc_cols - vc->vc_x)
		nr = vc->vc_cols - vc->vc_x;
	else if (!nr)
		nr = 1;
	delete_char(vc, nr);
}