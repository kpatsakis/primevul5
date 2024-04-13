GF_Err ireftype_box_size(GF_Box *s)
{
	GF_ItemReferenceTypeBox *ptr = (GF_ItemReferenceTypeBox *)s;
	ptr->size += 4 + (ptr->reference_count * sizeof(u16));
	return GF_OK;
}