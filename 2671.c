void bxml_box_del(GF_Box *s)
{
	GF_BinaryXMLBox *ptr = (GF_BinaryXMLBox *)s;
	if (ptr == NULL) return;
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}