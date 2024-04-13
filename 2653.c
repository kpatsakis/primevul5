GF_Err meta_box_size(GF_Box *s)
{
	u32 pos=0;
	GF_MetaBox *ptr = (GF_MetaBox *)s;

	gf_isom_check_position(s, (GF_Box *)ptr->handler, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->primary_resource, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->file_locations, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->item_locations, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->protections, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->item_infos, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->item_refs, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->item_props, &pos);
	gf_isom_check_position(s, (GF_Box *)ptr->groups_list, &pos);
	return GF_OK;
}