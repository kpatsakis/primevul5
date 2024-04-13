GF_Box *meta_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MetaBox, GF_ISOM_BOX_TYPE_META);
	tmp->child_boxes = gf_list_new();
	return (GF_Box *)tmp;
}