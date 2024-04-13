static void input_dev_release_keys(struct input_dev *dev)
{
	bool need_sync = false;
	int code;

	if (is_event_supported(EV_KEY, dev->evbit, EV_MAX)) {
		for_each_set_bit(code, dev->key, KEY_CNT) {
			input_pass_event(dev, EV_KEY, code, 0);
			need_sync = true;
		}

		if (need_sync)
			input_pass_event(dev, EV_SYN, SYN_REPORT, 1);

		memset(dev->key, 0, sizeof(dev->key));
	}
}