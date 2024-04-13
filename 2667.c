GF_Err iref_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_ItemReferenceBox *ptr = (GF_ItemReferenceBox *)s;
	gf_isom_check_position_list(s, ptr->references, &pos);
	return GF_OK;
}