static bool match_keycode(struct hid_usage *usage,
			  unsigned int cur_idx, unsigned int keycode)
{
	/*
	 * We should exclude unmapped usages when doing lookup by keycode.
	 */
	return (usage->type == EV_KEY && usage->code == keycode);
}