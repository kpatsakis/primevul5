GF_Err iinf_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ItemInfoBox *ptr = (GF_ItemInfoBox *)s;

	if (ptr->version == 0) {
		ISOM_DECREASE_SIZE(s, 2)
		gf_bs_read_u16(bs);
	} else {
		ISOM_DECREASE_SIZE(s, 4)
		gf_bs_read_u32(bs);
	}
	return gf_isom_box_array_read(s, bs);
}