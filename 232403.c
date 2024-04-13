static int mt_process_slot(struct mt_device *td, struct input_dev *input,
			    struct mt_application *app,
			    struct mt_usages *slot)
{
	struct input_mt *mt = input->mt;
	__s32 quirks = app->quirks;
	bool valid = true;
	bool confidence_state = true;
	bool inrange_state = false;
	int active;
	int slotnum;
	int tool = MT_TOOL_FINGER;

	if (!slot)
		return -EINVAL;

	if ((quirks & MT_QUIRK_CONTACT_CNT_ACCURATE) &&
	    app->num_received >= app->num_expected)
		return -EAGAIN;

	if (!(quirks & MT_QUIRK_ALWAYS_VALID)) {
		if (quirks & MT_QUIRK_VALID_IS_INRANGE)
			valid = *slot->inrange_state;
		if (quirks & MT_QUIRK_NOT_SEEN_MEANS_UP)
			valid = *slot->tip_state;
		if (quirks & MT_QUIRK_VALID_IS_CONFIDENCE)
			valid = *slot->confidence_state;

		if (!valid)
			return 0;
	}

	slotnum = mt_compute_slot(td, app, slot, input);
	if (slotnum < 0 || slotnum >= td->maxcontacts)
		return 0;

	if ((quirks & MT_QUIRK_IGNORE_DUPLICATES) && mt) {
		struct input_mt_slot *i_slot = &mt->slots[slotnum];

		if (input_mt_is_active(i_slot) &&
		    input_mt_is_used(mt, i_slot))
			return -EAGAIN;
	}

	if (quirks & MT_QUIRK_CONFIDENCE)
		confidence_state = *slot->confidence_state;

	if (quirks & MT_QUIRK_HOVERING)
		inrange_state = *slot->inrange_state;

	active = *slot->tip_state || inrange_state;

	if (app->application == HID_GD_SYSTEM_MULTIAXIS)
		tool = MT_TOOL_DIAL;
	else if (unlikely(!confidence_state)) {
		tool = MT_TOOL_PALM;
		if (!active && mt &&
		    input_mt_is_active(&mt->slots[slotnum])) {
			/*
			 * The non-confidence was reported for
			 * previously valid contact that is also no
			 * longer valid. We can't simply report
			 * lift-off as userspace will not be aware
			 * of non-confidence, so we need to split
			 * it into 2 events: active MT_TOOL_PALM
			 * and a separate liftoff.
			 */
			active = true;
			set_bit(slotnum, app->pending_palm_slots);
		}
	}

	input_mt_slot(input, slotnum);
	input_mt_report_slot_state(input, tool, active);
	if (active) {
		/* this finger is in proximity of the sensor */
		int wide = (*slot->w > *slot->h);
		int major = max(*slot->w, *slot->h);
		int minor = min(*slot->w, *slot->h);
		int orientation = wide;
		int max_azimuth;
		int azimuth;

		if (slot->a != DEFAULT_ZERO) {
			/*
			 * Azimuth is counter-clockwise and ranges from [0, MAX)
			 * (a full revolution). Convert it to clockwise ranging
			 * [-MAX/2, MAX/2].
			 *
			 * Note that ABS_MT_ORIENTATION require us to report
			 * the limit of [-MAX/4, MAX/4], but the value can go
			 * out of range to [-MAX/2, MAX/2] to report an upside
			 * down ellipsis.
			 */
			azimuth = *slot->a;
			max_azimuth = input_abs_get_max(input,
							ABS_MT_ORIENTATION);
			if (azimuth > max_azimuth * 2)
				azimuth -= max_azimuth * 4;
			orientation = -azimuth;
		}

		if (quirks & MT_QUIRK_TOUCH_SIZE_SCALING) {
			/*
			 * divided by two to match visual scale of touch
			 * for devices with this quirk
			 */
			major = major >> 1;
			minor = minor >> 1;
		}

		input_event(input, EV_ABS, ABS_MT_POSITION_X, *slot->x);
		input_event(input, EV_ABS, ABS_MT_POSITION_Y, *slot->y);
		input_event(input, EV_ABS, ABS_MT_TOOL_X, *slot->cx);
		input_event(input, EV_ABS, ABS_MT_TOOL_Y, *slot->cy);
		input_event(input, EV_ABS, ABS_MT_DISTANCE, !*slot->tip_state);
		input_event(input, EV_ABS, ABS_MT_ORIENTATION, orientation);
		input_event(input, EV_ABS, ABS_MT_PRESSURE, *slot->p);
		input_event(input, EV_ABS, ABS_MT_TOUCH_MAJOR, major);
		input_event(input, EV_ABS, ABS_MT_TOUCH_MINOR, minor);

		set_bit(MT_IO_FLAGS_ACTIVE_SLOTS, &td->mt_io_flags);
	}

	return 0;
}