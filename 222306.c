void input_enable_softrepeat(struct input_dev *dev, int delay, int period)
{
	dev->timer.function = input_repeat_key;
	dev->rep[REP_DELAY] = delay;
	dev->rep[REP_PERIOD] = period;
}