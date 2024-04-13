int input_get_new_minor(int legacy_base, unsigned int legacy_num,
			bool allow_dynamic)
{
	/*
	 * This function should be called from input handler's ->connect()
	 * methods, which are serialized with input_mutex, so no additional
	 * locking is needed here.
	 */
	if (legacy_base >= 0) {
		int minor = ida_simple_get(&input_ida,
					   legacy_base,
					   legacy_base + legacy_num,
					   GFP_KERNEL);
		if (minor >= 0 || !allow_dynamic)
			return minor;
	}

	return ida_simple_get(&input_ida,
			      INPUT_FIRST_DYNAMIC_DEV, INPUT_MAX_CHAR_DEVICES,
			      GFP_KERNEL);
}