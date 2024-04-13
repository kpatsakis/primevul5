static void vc_uniscr_set(struct vc_data *vc, struct uni_screen *new_uniscr)
{
	kfree(vc->vc_uni_screen);
	vc->vc_uni_screen = new_uniscr;
}