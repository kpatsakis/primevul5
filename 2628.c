GF_Box *xml_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_XMLBox, GF_ISOM_BOX_TYPE_XML);
	return (GF_Box *)tmp;
}