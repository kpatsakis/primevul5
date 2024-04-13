GF_Err iinf_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_ItemInfoBox *ptr = (GF_ItemInfoBox *)s;

	if (a->type == GF_ISOM_BOX_TYPE_INFE) {
		BOX_FIELD_LIST_ASSIGN(item_infos)
		return GF_OK;
	} else {
		return GF_OK;
	}
}