void xml_box_del(GF_Box *s)
{
	GF_XMLBox *ptr = (GF_XMLBox *)s;
	if (ptr == NULL) return;
	if (ptr->xml) gf_free(ptr->xml);
	gf_free(ptr);
}