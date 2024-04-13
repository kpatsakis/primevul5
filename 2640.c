void meta_box_del(GF_Box *s)
{
	meta_reset(s);
	gf_free(s);
}