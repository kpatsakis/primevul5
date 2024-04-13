void pitm_box_del(GF_Box *s)
{
	GF_PrimaryItemBox *ptr = (GF_PrimaryItemBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}