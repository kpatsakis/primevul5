iscsi_create_endpoint(int dd_size)
{
	struct device *dev;
	struct iscsi_endpoint *ep;
	uint64_t id;
	int err;

	for (id = 1; id < ISCSI_MAX_EPID; id++) {
		dev = class_find_device(&iscsi_endpoint_class, NULL, &id,
					iscsi_match_epid);
		if (!dev)
			break;
		else
			put_device(dev);
	}
	if (id == ISCSI_MAX_EPID) {
		printk(KERN_ERR "Too many connections. Max supported %u\n",
		       ISCSI_MAX_EPID - 1);
		return NULL;
	}

	ep = kzalloc(sizeof(*ep) + dd_size, GFP_KERNEL);
	if (!ep)
		return NULL;

	ep->id = id;
	ep->dev.class = &iscsi_endpoint_class;
	dev_set_name(&ep->dev, "ep-%llu", (unsigned long long) id);
	err = device_register(&ep->dev);
        if (err)
                goto free_ep;

	err = sysfs_create_group(&ep->dev.kobj, &iscsi_endpoint_group);
	if (err)
		goto unregister_dev;

	if (dd_size)
		ep->dd_data = &ep[1];
	return ep;

unregister_dev:
	device_unregister(&ep->dev);
	return NULL;

free_ep:
	kfree(ep);
	return NULL;
}