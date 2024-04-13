GF_Err ipro_box_size(GF_Box *s)
{
	GF_ItemProtectionBox *ptr = (GF_ItemProtectionBox *)s;
	u32 pos=0;
	gf_isom_check_position_list(s, ptr->protection_information, &pos);
	s->size += 2;
	return GF_OK;
}