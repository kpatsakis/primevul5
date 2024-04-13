ms_escher_header_add_attr (MSEscherHeader *h, MSObjAttr *attr)
{
	if (h->attrs == NULL)
		h->attrs = ms_obj_attr_bag_new ();
	ms_obj_attr_bag_insert (h->attrs, attr);
}