static struct hid_usage *hidinput_locate_usage(struct hid_device *hid,
					const struct input_keymap_entry *ke,
					unsigned int *index)
{
	struct hid_usage *usage;
	unsigned int scancode;

	if (ke->flags & INPUT_KEYMAP_BY_INDEX)
		usage = hidinput_find_key(hid, match_index, ke->index, index);
	else if (input_scancode_to_scalar(ke, &scancode) == 0)
		usage = hidinput_find_key(hid, match_scancode, scancode, index);
	else
		usage = NULL;

	return usage;
}