GF_Err iloc_box_size(GF_Box *s)
{
	u32 i, item_count, extent_count;
	GF_ItemLocationBox *ptr = (GF_ItemLocationBox *)s;
	if (!s) return GF_BAD_PARAM;
	if (ptr->index_size) {
		ptr->version = 1;
	}
	item_count = gf_list_count(ptr->location_entries);
	for (i = 0; i < item_count; i++) {
		GF_ItemLocationEntry *location = (GF_ItemLocationEntry *)gf_list_get(ptr->location_entries, i);
		if (location->construction_method) {
			ptr->version = 1;
		}
		if (location->item_ID > 0xFFFF) {
			ptr->version = 2;
		}
	}
	ptr->size += 4;
	if (ptr->version == 2) {
		ptr->size += 2; // 32 bits item count instead of 16 bits
	}
	for (i = 0; i < item_count; i++) {
		GF_ItemLocationEntry *location = (GF_ItemLocationEntry *)gf_list_get(ptr->location_entries, i);
		extent_count = gf_list_count(location->extent_entries);
		ptr->size += 6 + ptr->base_offset_size + extent_count*(ptr->offset_size + ptr->length_size);
		if (ptr->version == 2) {
			ptr->size += 2; //32 bits item ID instead of 16 bits
		}
		if (ptr->version == 1 || ptr->version == 2) {
			ptr->size += 2; // construction_method
			ptr->size += extent_count*ptr->index_size;
		}
	}
	return GF_OK;
}