static struct mt_usages *mt_allocate_usage(struct hid_device *hdev,
					   struct mt_application *application)
{
	struct mt_usages *usage;

	usage = devm_kzalloc(&hdev->dev, sizeof(*usage), GFP_KERNEL);
	if (!usage)
		return NULL;

	/* set some defaults so we do not need to check for null pointers */
	usage->x = DEFAULT_ZERO;
	usage->y = DEFAULT_ZERO;
	usage->cx = DEFAULT_ZERO;
	usage->cy = DEFAULT_ZERO;
	usage->p = DEFAULT_ZERO;
	usage->w = DEFAULT_ZERO;
	usage->h = DEFAULT_ZERO;
	usage->a = DEFAULT_ZERO;
	usage->contactid = DEFAULT_ZERO;
	usage->tip_state = DEFAULT_FALSE;
	usage->inrange_state = DEFAULT_FALSE;
	usage->confidence_state = DEFAULT_TRUE;

	list_add_tail(&usage->list, &application->mt_usages);

	return usage;
}