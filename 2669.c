void iref_box_del(GF_Box *s)
{
	GF_ItemReferenceBox *ptr = (GF_ItemReferenceBox *)s;
	if (ptr == NULL) return;
	gf_list_del(ptr->references);
	gf_free(ptr);
}