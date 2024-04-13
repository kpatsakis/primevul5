static void remove_config(struct usb_composite_dev *cdev,
			      struct usb_configuration *config)
{
	while (!list_empty(&config->functions)) {
		struct usb_function		*f;

		f = list_first_entry(&config->functions,
				struct usb_function, list);

		usb_remove_function(config, f);
	}
	list_del(&config->list);
	if (config->unbind) {
		DBG(cdev, "unbind config '%s'/%p\n", config->label, config);
		config->unbind(config);
			/* may free memory for "c" */
	}
}