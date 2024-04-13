void iinf_box_del(GF_Box *s)
{
	GF_ItemInfoBox *ptr = (GF_ItemInfoBox *)s;
	if (ptr == NULL) return;
	gf_list_del(ptr->item_infos);
	gf_free(ptr);
}