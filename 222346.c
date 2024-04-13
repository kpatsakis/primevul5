static void input_stop_autorepeat(struct input_dev *dev)
{
	del_timer(&dev->timer);
}