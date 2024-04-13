GF_Err ipro_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_ItemProtectionBox *ptr = (GF_ItemProtectionBox *)s;
	if (a->type == GF_ISOM_BOX_TYPE_SINF) {
		BOX_FIELD_LIST_ASSIGN(protection_information)
		return GF_OK;
	}
	return GF_OK;
}