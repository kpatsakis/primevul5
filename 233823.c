ms_escher_header_release (MSEscherHeader *h)
{
	if (h->attrs != NULL) {
		if (h->release_attrs)
			ms_obj_attr_bag_destroy (h->attrs);
		h->attrs = NULL;
	}
}