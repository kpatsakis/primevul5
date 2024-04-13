static unsigned int input_fetch_keycode(struct input_dev *dev,
					unsigned int index)
{
	switch (dev->keycodesize) {
	case 1:
		return ((u8 *)dev->keycode)[index];

	case 2:
		return ((u16 *)dev->keycode)[index];

	default:
		return ((u32 *)dev->keycode)[index];
	}
}