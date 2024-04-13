void infe_box_del(GF_Box *s)
{
	GF_ItemInfoEntryBox *ptr = (GF_ItemInfoEntryBox *)s;
	if (ptr == NULL) return;
	if (ptr->item_name) gf_free(ptr->item_name);
	if (ptr->full_path) gf_free(ptr->full_path);
	if (ptr->content_type) gf_free(ptr->content_type);
	if (ptr->content_encoding) gf_free(ptr->content_encoding);
	gf_free(ptr);
}