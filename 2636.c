GF_Err meta_on_child_box(GF_Box *s, GF_Box *a, Bool is_rem)
{
	GF_MetaBox *ptr = (GF_MetaBox *)s;
	switch (a->type) {
	case GF_ISOM_BOX_TYPE_HDLR:
		BOX_FIELD_ASSIGN(handler, GF_HandlerBox)
		break;
	case GF_ISOM_BOX_TYPE_PITM:
		BOX_FIELD_ASSIGN(primary_resource, GF_PrimaryItemBox)
		break;
	case GF_ISOM_BOX_TYPE_DINF:
		BOX_FIELD_ASSIGN(file_locations, GF_DataInformationBox)
		break;
	case GF_ISOM_BOX_TYPE_ILOC:
		BOX_FIELD_ASSIGN(item_locations, GF_ItemLocationBox)
		break;
	case GF_ISOM_BOX_TYPE_IPRO:
		BOX_FIELD_ASSIGN(protections, GF_ItemProtectionBox)
		break;
	case GF_ISOM_BOX_TYPE_IINF:
		BOX_FIELD_ASSIGN(item_infos, GF_ItemInfoBox)
		break;
	case GF_ISOM_BOX_TYPE_IREF:
		BOX_FIELD_ASSIGN(item_refs, GF_ItemReferenceBox);
		break;
	case GF_ISOM_BOX_TYPE_IPRP:
		BOX_FIELD_ASSIGN(item_props, GF_ItemPropertiesBox)
		break;
	case GF_ISOM_BOX_TYPE_GRPL:
		BOX_FIELD_ASSIGN(groups_list, GF_GroupListBox)
		break;
	}
	return GF_OK;
}