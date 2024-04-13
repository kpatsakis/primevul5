GF_Err xml_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_XMLBox *ptr = (GF_XMLBox *)s;
	return gf_isom_read_null_terminated_string(s, bs, s->size, &ptr->xml);
}