void meta_reset(GF_Box *s)
{
	GF_MetaBox *ptr = (GF_MetaBox *)s;
	if (ptr == NULL) return;

	//destroy all boxes
	if (ptr->child_boxes) gf_isom_box_array_del(ptr->child_boxes);
	ptr->child_boxes = NULL;

	//reset pointers (destroyed above)
	ptr->handler = NULL;
	ptr->primary_resource = NULL;
	ptr->file_locations = NULL;
	ptr->item_locations = NULL;
	ptr->protections = NULL;
	ptr->item_infos = NULL;
	ptr->IPMP_control = NULL;
	ptr->item_refs = NULL;
	ptr->item_props = NULL;
	ptr->groups_list = NULL;
}