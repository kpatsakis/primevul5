int usb_add_config_only(struct usb_composite_dev *cdev,
		struct usb_configuration *config)
{
	struct usb_configuration *c;

	if (!config->bConfigurationValue)
		return -EINVAL;

	/* Prevent duplicate configuration identifiers */
	list_for_each_entry(c, &cdev->configs, list) {
		if (c->bConfigurationValue == config->bConfigurationValue)
			return -EBUSY;
	}

	config->cdev = cdev;
	list_add_tail(&config->list, &cdev->configs);

	INIT_LIST_HEAD(&config->functions);
	config->next_interface_id = 0;
	memset(config->interface, 0, sizeof(config->interface));

	return 0;
}