static int rtsx_usb_ms_set_param(struct memstick_host *msh,
		enum memstick_param param, int value)
{
	struct rtsx_usb_ms *host = memstick_priv(msh);
	struct rtsx_ucr *ucr = host->ucr;
	unsigned int clock = 0;
	u8 ssc_depth = 0;
	int err;

	dev_dbg(ms_dev(host), "%s: param = %d, value = %d\n",
			__func__, param, value);

	pm_runtime_get_sync(ms_dev(host));
	mutex_lock(&ucr->dev_mutex);

	err = rtsx_usb_card_exclusive_check(ucr, RTSX_USB_MS_CARD);
	if (err)
		goto out;

	switch (param) {
	case MEMSTICK_POWER:
		if (value == host->power_mode)
			break;

		if (value == MEMSTICK_POWER_ON) {
			pm_runtime_get_noresume(ms_dev(host));
			err = ms_power_on(host);
			if (err)
				pm_runtime_put_noidle(ms_dev(host));
		} else if (value == MEMSTICK_POWER_OFF) {
			err = ms_power_off(host);
			if (!err)
				pm_runtime_put_noidle(ms_dev(host));
		} else
			err = -EINVAL;
		if (!err)
			host->power_mode = value;
		break;

	case MEMSTICK_INTERFACE:
		if (value == MEMSTICK_SERIAL) {
			clock = 19000000;
			ssc_depth = SSC_DEPTH_512K;
			err = rtsx_usb_write_register(ucr, MS_CFG, 0x5A,
				       MS_BUS_WIDTH_1 | PUSH_TIME_DEFAULT);
			if (err < 0)
				break;
		} else if (value == MEMSTICK_PAR4) {
			clock = 39000000;
			ssc_depth = SSC_DEPTH_1M;

			err = rtsx_usb_write_register(ucr, MS_CFG, 0x5A,
					MS_BUS_WIDTH_4 | PUSH_TIME_ODD |
					MS_NO_CHECK_INT);
			if (err < 0)
				break;
		} else {
			err = -EINVAL;
			break;
		}

		err = rtsx_usb_switch_clock(ucr, clock,
				ssc_depth, false, true, false);
		if (err < 0) {
			dev_dbg(ms_dev(host), "switch clock failed\n");
			break;
		}

		host->ssc_depth = ssc_depth;
		host->clock = clock;
		host->ifmode = value;
		break;
	default:
		err = -EINVAL;
		break;
	}
out:
	mutex_unlock(&ucr->dev_mutex);
	pm_runtime_put_sync(ms_dev(host));

	/* power-on delay */
	if (param == MEMSTICK_POWER && value == MEMSTICK_POWER_ON) {
		usleep_range(10000, 12000);

		if (!host->eject)
			schedule_delayed_work(&host->poll_card, 100);
	}

	dev_dbg(ms_dev(host), "%s: return = %d\n", __func__, err);
	return err;
}