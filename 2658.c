GF_Err bxml_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_BinaryXMLBox *ptr = (GF_BinaryXMLBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->data_length) gf_bs_write_data(bs, ptr->data, ptr->data_length);
	return GF_OK;
}