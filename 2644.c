GF_Err iinf_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_ItemInfoBox *ptr = (GF_ItemInfoBox *)s;
	if (!s) return GF_BAD_PARAM;
	ptr->size += (ptr->version == 0) ? 2 : 4;
	gf_isom_check_position_list(s, ptr->item_infos, &pos);
	return GF_OK;
}