GF_Box *infe_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ItemInfoEntryBox, GF_ISOM_BOX_TYPE_INFE);
	return (GF_Box *)tmp;
}