GF_Err ireftype_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_ItemReferenceTypeBox *ptr = (GF_ItemReferenceTypeBox *)s;
	ptr->type = ptr->reference_type;
	e = gf_isom_box_write_header(s, bs);
	ptr->type = GF_ISOM_BOX_TYPE_REFI;
	if (e) return e;
	gf_bs_write_u16(bs, ptr->from_item_id);
	gf_bs_write_u16(bs, ptr->reference_count);
	for (i = 0; i < ptr->reference_count; i++) {
		gf_bs_write_u16(bs, ptr->to_item_IDs[i]);
	}
	return GF_OK;
}