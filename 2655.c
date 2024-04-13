GF_Err xml_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_XMLBox *ptr = (GF_XMLBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->xml) gf_bs_write_data(bs, ptr->xml, (u32) strlen(ptr->xml));
	gf_bs_write_u8(bs, 0);
	return GF_OK;
}