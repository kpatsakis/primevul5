struct input_dev *input_allocate_device(void)
{
	static atomic_t input_no = ATOMIC_INIT(-1);
	struct input_dev *dev;

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (dev) {
		dev->dev.type = &input_dev_type;
		dev->dev.class = &input_class;
		device_initialize(&dev->dev);
		mutex_init(&dev->mutex);
		spin_lock_init(&dev->event_lock);
		timer_setup(&dev->timer, NULL, 0);
		INIT_LIST_HEAD(&dev->h_list);
		INIT_LIST_HEAD(&dev->node);

		dev_set_name(&dev->dev, "input%lu",
			     (unsigned long)atomic_inc_return(&input_no));

		__module_get(THIS_MODULE);
	}

	return dev;
}