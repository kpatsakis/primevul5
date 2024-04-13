GF_Box *iinf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ItemInfoBox, GF_ISOM_BOX_TYPE_IINF);
	tmp->item_infos = gf_list_new();
	return (GF_Box *)tmp;
}