GF_Box *iref_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ItemReferenceBox, GF_ISOM_BOX_TYPE_IREF);
	tmp->references = gf_list_new();
	return (GF_Box *)tmp;
}