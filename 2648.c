void ipro_box_del(GF_Box *s)
{
	GF_ItemProtectionBox *ptr = (GF_ItemProtectionBox *)s;
	if (ptr == NULL) return;
	gf_list_del(ptr->protection_information);
	gf_free(ptr);
}