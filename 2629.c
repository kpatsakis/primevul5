GF_Err infe_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 len;
	GF_ItemInfoEntryBox *ptr = (GF_ItemInfoEntryBox *)s;
	if (!s) return GF_BAD_PARAM;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->version == 3) {
		gf_bs_write_u32(bs, ptr->item_ID);
	}
	else {
		gf_bs_write_u16(bs, ptr->item_ID);
	}
	gf_bs_write_u16(bs, ptr->item_protection_index);
	if (ptr->version >= 2) {
		gf_bs_write_u32(bs, ptr->item_type);
	}
	if (ptr->item_name) {
		len = (u32) strlen(ptr->item_name)+1;
		gf_bs_write_data(bs, ptr->item_name, len);
	} else {
		gf_bs_write_byte(bs, 0, 1);
	}
	if (ptr->item_type == GF_META_ITEM_TYPE_MIME || ptr->item_type == GF_META_ITEM_TYPE_URI) {
		if (ptr->content_type) {
			len = (u32)strlen(ptr->content_type) + 1;
			gf_bs_write_data(bs, ptr->content_type, len);
		}
		else {
			gf_bs_write_byte(bs, 0, 1);
		}
	}
	if (ptr->item_type == GF_META_ITEM_TYPE_MIME) {
		if (ptr->content_encoding) {
			len = (u32)strlen(ptr->content_encoding) + 1;
			gf_bs_write_data(bs, ptr->content_encoding, len);
		}
		else {
			gf_bs_write_byte(bs, 0, 1);
		}
	}
	return GF_OK;
}