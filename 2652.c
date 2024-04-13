GF_Box *pitm_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_PrimaryItemBox, GF_ISOM_BOX_TYPE_PITM);
	return (GF_Box *)tmp;
}