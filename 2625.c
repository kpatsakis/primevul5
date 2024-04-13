void ireftype_box_del(GF_Box *s)
{
	GF_ItemReferenceTypeBox *ptr = (GF_ItemReferenceTypeBox *)s;
	if (!ptr) return;
	if (ptr->to_item_IDs) gf_free(ptr->to_item_IDs);
	gf_free(ptr);
}