static void mt_process_mt_event(struct hid_device *hid,
				struct mt_application *app,
				struct hid_field *field,
				struct hid_usage *usage,
				__s32 value,
				bool first_packet)
{
	__s32 quirks = app->quirks;
	struct input_dev *input = field->hidinput->input;

	if (!usage->type || !(hid->claimed & HID_CLAIMED_INPUT))
		return;

	if (quirks & MT_QUIRK_WIN8_PTP_BUTTONS) {

		/*
		 * For Win8 PTP touchpads we should only look at
		 * non finger/touch events in the first_packet of a
		 * (possible) multi-packet frame.
		 */
		if (!first_packet)
			return;

		/*
		 * For Win8 PTP touchpads we map both the clickpad click
		 * and any "external" left buttons to BTN_LEFT if a
		 * device claims to have both we need to report 1 for
		 * BTN_LEFT if either is pressed, so we or all values
		 * together and report the result in mt_sync_frame().
		 */
		if (usage->type == EV_KEY && usage->code == BTN_LEFT) {
			app->left_button_state |= value;
			return;
		}
	}

	input_event(input, usage->type, usage->code, value);
}