static void mt_store_field(struct hid_device *hdev,
			   struct mt_application *application,
			   __s32 *value,
			   size_t offset)
{
	struct mt_usages *usage;
	__s32 **target;

	if (list_empty(&application->mt_usages))
		usage = mt_allocate_usage(hdev, application);
	else
		usage = list_last_entry(&application->mt_usages,
					struct mt_usages,
					list);

	if (!usage)
		return;

	target = (__s32 **)((char *)usage + offset);

	/* the value has already been filled, create a new slot */
	if (*target != DEFAULT_TRUE &&
	    *target != DEFAULT_FALSE &&
	    *target != DEFAULT_ZERO) {
		if (usage->contactid == DEFAULT_ZERO ||
		    usage->x == DEFAULT_ZERO ||
		    usage->y == DEFAULT_ZERO) {
			hid_dbg(hdev,
				"ignoring duplicate usage on incomplete");
			return;
		}
		usage = mt_allocate_usage(hdev, application);
		if (!usage)
			return;

		target = (__s32 **)((char *)usage + offset);
	}

	*target = value;
}