GF_Err iref_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_ItemReferenceBox *ptr = (GF_ItemReferenceBox *)s;
	BOX_FIELD_LIST_ASSIGN(references)
	return GF_OK;
}