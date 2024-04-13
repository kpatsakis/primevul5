GF_Box *bxml_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_BinaryXMLBox, GF_ISOM_BOX_TYPE_BXML);
	return (GF_Box *)tmp;
}