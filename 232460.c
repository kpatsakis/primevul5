static int mt_compute_timestamp(struct mt_application *app, __s32 value)
{
	long delta = value - app->prev_scantime;
	unsigned long jdelta = jiffies_to_usecs(jiffies - app->jiffies);

	app->jiffies = jiffies;

	if (delta < 0)
		delta += app->scantime_logical_max;

	/* HID_DG_SCANTIME is expressed in 100us, we want it in us. */
	delta *= 100;

	if (jdelta > MAX_TIMESTAMP_INTERVAL)
		/* No data received for a while, resync the timestamp. */
		return 0;
	else
		return app->timestamp + delta;
}