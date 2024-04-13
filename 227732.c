static struct uvc_entity *uvc_entity_by_reference(struct uvc_device *dev,
	int id, struct uvc_entity *entity)
{
	unsigned int i;

	if (entity == NULL)
		entity = list_entry(&dev->entities, struct uvc_entity, list);

	list_for_each_entry_continue(entity, &dev->entities, list) {
		for (i = 0; i < entity->bNrInPins; ++i)
			if (entity->baSourceID[i] == id)
				return entity;
	}

	return NULL;
}