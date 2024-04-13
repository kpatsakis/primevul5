static struct uvc_entity *uvc_alloc_entity(u16 type, u8 id,
		unsigned int num_pads, unsigned int extra_size)
{
	struct uvc_entity *entity;
	unsigned int num_inputs;
	unsigned int size;
	unsigned int i;

	extra_size = roundup(extra_size, sizeof(*entity->pads));
	num_inputs = (type & UVC_TERM_OUTPUT) ? num_pads : num_pads - 1;
	size = sizeof(*entity) + extra_size + sizeof(*entity->pads) * num_pads
	     + num_inputs;
	entity = kzalloc(size, GFP_KERNEL);
	if (entity == NULL)
		return NULL;

	entity->id = id;
	entity->type = type;

	entity->num_links = 0;
	entity->num_pads = num_pads;
	entity->pads = ((void *)(entity + 1)) + extra_size;

	for (i = 0; i < num_inputs; ++i)
		entity->pads[i].flags = MEDIA_PAD_FL_SINK;
	if (!UVC_ENTITY_IS_OTERM(entity))
		entity->pads[num_pads-1].flags = MEDIA_PAD_FL_SOURCE;

	entity->bNrInPins = num_inputs;
	entity->baSourceID = (u8 *)(&entity->pads[num_pads]);

	return entity;
}