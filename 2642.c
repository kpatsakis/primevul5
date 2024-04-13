GF_Box *ipro_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ItemProtectionBox, GF_ISOM_BOX_TYPE_IPRO);
	tmp->protection_information = gf_list_new();
	return (GF_Box *)tmp;
}