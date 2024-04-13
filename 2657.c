void iloc_entry_del(GF_ItemLocationEntry *location)
{
	u32 j, extent_count;
	extent_count = gf_list_count(location->extent_entries);
	for (j = 0; j < extent_count; j++) {
		GF_ItemExtentEntry *extent = (GF_ItemExtentEntry *)gf_list_get(location->extent_entries, j);
		gf_free(extent);
	}
	gf_list_del(location->extent_entries);
	gf_free(location);
}