GF_Err ipro_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 count;
	GF_Err e;
	GF_ItemProtectionBox *ptr = (GF_ItemProtectionBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	count = gf_list_count(ptr->protection_information);
	gf_bs_write_u16(bs, count);
	return GF_OK;
}