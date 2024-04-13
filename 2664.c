GF_Err pitm_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_PrimaryItemBox *ptr = (GF_PrimaryItemBox *)s;
	ISOM_DECREASE_SIZE(ptr, 2)
	ptr->item_ID = gf_bs_read_u16(bs);
	return GF_OK;
}