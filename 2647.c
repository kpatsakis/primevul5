GF_Err iloc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i, j, item_count, extent_count;
	GF_ItemLocationBox *ptr = (GF_ItemLocationBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_int(bs, ptr->offset_size, 4);
	gf_bs_write_int(bs, ptr->length_size, 4);
	gf_bs_write_int(bs, ptr->base_offset_size, 4);
	gf_bs_write_int(bs, ptr->index_size, 4);
	item_count = gf_list_count(ptr->location_entries);
	if (ptr->version < 2) {
		gf_bs_write_u16(bs, item_count);
	}
	else {
		gf_bs_write_u32(bs, item_count);
	}
	for (i = 0; i < item_count; i++) {
		GF_ItemLocationEntry *location = (GF_ItemLocationEntry *)gf_list_get(ptr->location_entries, i);
		if (ptr->version < 2) {
			gf_bs_write_u16(bs, location->item_ID);
		}
		else {
			gf_bs_write_u32(bs, location->item_ID);
		}
		if (ptr->version == 1 || ptr->version == 2) {
			gf_bs_write_u16(bs, location->construction_method);
		}
		gf_bs_write_u16(bs, location->data_reference_index);
		gf_bs_write_long_int(bs, location->base_offset, 8*ptr->base_offset_size);
		extent_count = gf_list_count(location->extent_entries);
		gf_bs_write_u16(bs, extent_count);
		for (j=0; j<extent_count; j++) {
			GF_ItemExtentEntry *extent = (GF_ItemExtentEntry *)gf_list_get(location->extent_entries, j);
			if ((ptr->version == 1 || ptr->version == 2) && ptr->index_size > 0) {
				gf_bs_write_long_int(bs, extent->extent_index, 8 * ptr->index_size);
			}
			gf_bs_write_long_int(bs, extent->extent_offset, 8*ptr->offset_size);
			gf_bs_write_long_int(bs, extent->extent_length, 8*ptr->length_size);
		}
	}
	return GF_OK;
}