void input_set_timestamp(struct input_dev *dev, ktime_t timestamp)
{
	dev->timestamp[INPUT_CLK_MONO] = timestamp;
	dev->timestamp[INPUT_CLK_REAL] = ktime_mono_to_real(timestamp);
	dev->timestamp[INPUT_CLK_BOOT] = ktime_mono_to_any(timestamp,
							   TK_OFFS_BOOT);
}