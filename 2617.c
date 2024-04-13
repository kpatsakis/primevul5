void iloc_box_del(GF_Box *s)
{
	u32 i, item_count;
	GF_ItemLocationBox *ptr = (GF_ItemLocationBox *)s;
	if (ptr == NULL) return;
	item_count = gf_list_count(ptr->location_entries);
	for (i = 0; i < item_count; i++) {
		GF_ItemLocationEntry *location = (GF_ItemLocationEntry *)gf_list_get(ptr->location_entries, i);
		iloc_entry_del(location);
	}
	gf_list_del(ptr->location_entries);
	gf_free(ptr);
}