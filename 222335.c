static const struct input_device_id *input_match_device(struct input_handler *handler,
							struct input_dev *dev)
{
	const struct input_device_id *id;

	for (id = handler->id_table; id->flags || id->driver_info; id++) {
		if (input_match_device_id(dev, id) &&
		    (!handler->match || handler->match(handler, dev))) {
			return id;
		}
	}

	return NULL;
}