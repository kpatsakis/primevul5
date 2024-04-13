GF_Err pitm_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_PrimaryItemBox *ptr = (GF_PrimaryItemBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u16(bs, ptr->item_ID);
	return GF_OK;
}