GF_Err xml_box_size(GF_Box *s)
{
	GF_XMLBox *ptr = (GF_XMLBox *)s;
	if (!s) return GF_BAD_PARAM;
    ptr->size += (ptr->xml ? strlen(ptr->xml) : 0) + 1;
	return GF_OK;
}