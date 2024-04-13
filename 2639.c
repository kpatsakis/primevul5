GF_Err bxml_box_size(GF_Box *s)
{
	GF_BinaryXMLBox *ptr = (GF_BinaryXMLBox *)s;
	if (!s) return GF_BAD_PARAM;
	ptr->size += ptr->data_length;
	return GF_OK;
}