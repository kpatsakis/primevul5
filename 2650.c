GF_Err pitm_box_size(GF_Box *s)
{
	GF_ItemLocationBox *ptr = (GF_ItemLocationBox *)s;
	if (!s) return GF_BAD_PARAM;
	ptr->size += 2;
	return GF_OK;
}