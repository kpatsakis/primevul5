static bool match_scancode(struct hid_usage *usage,
			   unsigned int cur_idx, unsigned int scancode)
{
	return (usage->hid & (HID_USAGE_PAGE | HID_USAGE)) == scancode;
}