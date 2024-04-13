GF_Err infe_box_size(GF_Box *s)
{
	GF_ItemInfoEntryBox *ptr = (GF_ItemInfoEntryBox *)s;
	if (!s) return GF_BAD_PARAM;
	if (ptr->item_type) {
		ptr->version = 2;
		if (ptr->item_ID > 0xFFFF) {
			ptr->version = 3;
		}
	}
	else {
		ptr->version = 0;
	}
	ptr->size += 4;
	if (ptr->version == 3) {
		ptr->size += 2; // item_ID on 32 bits (+2 bytes)
	}
	if (ptr->version >= 2) {
		ptr->size += 4; // item_type size
	}
	if (ptr->item_name) ptr->size += strlen(ptr->item_name)+1;
	else ptr->size += 1;
	if (ptr->item_type == GF_META_ITEM_TYPE_MIME || ptr->item_type == GF_META_ITEM_TYPE_URI) {
		if (ptr->content_type) ptr->size += strlen(ptr->content_type) + 1;
		else ptr->size += 1;
	}
	if (ptr->item_type == GF_META_ITEM_TYPE_MIME) {
		if (ptr->content_encoding) ptr->size += strlen(ptr->content_encoding) + 1;
		else ptr->size += 1;
	}
	return GF_OK;
}