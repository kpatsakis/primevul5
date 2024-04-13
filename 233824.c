ms_escher_header_init (MSEscherHeader *h)
{
	h->ver = h->instance = h->fbt = h->len = 0;
	h->attrs = NULL;
	h->release_attrs = TRUE;
}