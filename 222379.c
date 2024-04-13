static unsigned int input_estimate_events_per_packet(struct input_dev *dev)
{
	int mt_slots;
	int i;
	unsigned int events;

	if (dev->mt) {
		mt_slots = dev->mt->num_slots;
	} else if (test_bit(ABS_MT_TRACKING_ID, dev->absbit)) {
		mt_slots = dev->absinfo[ABS_MT_TRACKING_ID].maximum -
			   dev->absinfo[ABS_MT_TRACKING_ID].minimum + 1,
		mt_slots = clamp(mt_slots, 2, 32);
	} else if (test_bit(ABS_MT_POSITION_X, dev->absbit)) {
		mt_slots = 2;
	} else {
		mt_slots = 0;
	}

	events = mt_slots + 1; /* count SYN_MT_REPORT and SYN_REPORT */

	if (test_bit(EV_ABS, dev->evbit))
		for_each_set_bit(i, dev->absbit, ABS_CNT)
			events += input_is_mt_axis(i) ? mt_slots : 1;

	if (test_bit(EV_REL, dev->evbit))
		events += bitmap_weight(dev->relbit, REL_CNT);

	/* Make room for KEY and MSC events */
	events += 7;

	return events;
}