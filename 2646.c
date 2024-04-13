GF_Box *iloc_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ItemLocationBox, GF_ISOM_BOX_TYPE_ILOC);
	tmp->location_entries = gf_list_new();
	return (GF_Box *)tmp;
}