GF_Err bxml_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_BinaryXMLBox *ptr = (GF_BinaryXMLBox *)s;

	ptr->data_length = (u32)(ptr->size);
	ptr->data = (char*)gf_malloc(sizeof(char)*ptr->data_length);
	if (!ptr->data) return GF_OUT_OF_MEM;
	gf_bs_read_data(bs, ptr->data, ptr->data_length);
	return GF_OK;
}