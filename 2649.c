GF_Box *ireftype_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ItemReferenceTypeBox, GF_ISOM_BOX_TYPE_REFI);
	return (GF_Box *)tmp;
}