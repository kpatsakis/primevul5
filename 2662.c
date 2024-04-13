GF_Err ipro_box_read(GF_Box *s, GF_BitStream *bs)
{
	ISOM_DECREASE_SIZE(s, 2)
	gf_bs_read_u16(bs);
	return gf_isom_box_array_read(s, bs);
}