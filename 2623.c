GF_Err iinf_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 count;
	GF_Err e;
	GF_ItemInfoBox *ptr = (GF_ItemInfoBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	count = gf_list_count(ptr->item_infos);
	if (ptr->version == 0)
		gf_bs_write_u16(bs, count);
	else
		gf_bs_write_u32(bs, count);

	return GF_OK;
}