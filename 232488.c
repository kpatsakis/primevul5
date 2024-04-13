static void hidinput_handle_scroll(struct hid_usage *usage,
				   struct input_dev *input,
				   __s32 value)
{
	int code;
	int hi_res, lo_res;

	if (value == 0)
		return;

	if (usage->code == REL_WHEEL_HI_RES)
		code = REL_WHEEL;
	else
		code = REL_HWHEEL;

	/*
	 * Windows reports one wheel click as value 120. Where a high-res
	 * scroll wheel is present, a fraction of 120 is reported instead.
	 * Our REL_WHEEL_HI_RES axis does the same because all HW must
	 * adhere to the 120 expectation.
	 */
	hi_res = value * 120/usage->resolution_multiplier;

	usage->wheel_accumulated += hi_res;
	lo_res = usage->wheel_accumulated/120;
	if (lo_res)
		usage->wheel_accumulated -= lo_res * 120;

	input_event(input, EV_REL, code, lo_res);
	input_event(input, EV_REL, usage->code, hi_res);
}