GF_Err ireftype_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_ItemReferenceTypeBox *ptr = (GF_ItemReferenceTypeBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->from_item_id = gf_bs_read_u16(bs);
	ptr->reference_count = gf_bs_read_u16(bs);
	if (ptr->size / 2 < ptr->reference_count)
		return GF_ISOM_INVALID_FILE;

	ptr->to_item_IDs = (u32 *)gf_malloc(ptr->reference_count * sizeof(u32));
	if (!ptr->to_item_IDs) return GF_OUT_OF_MEM;

	for (i=0; i < ptr->reference_count; i++) {
		ISOM_DECREASE_SIZE(ptr, 2)
		ptr->to_item_IDs[i] = gf_bs_read_u16(bs);
	}
	return GF_OK;
}