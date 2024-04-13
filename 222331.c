static int input_devices_seq_show(struct seq_file *seq, void *v)
{
	struct input_dev *dev = container_of(v, struct input_dev, node);
	const char *path = kobject_get_path(&dev->dev.kobj, GFP_KERNEL);
	struct input_handle *handle;

	seq_printf(seq, "I: Bus=%04x Vendor=%04x Product=%04x Version=%04x\n",
		   dev->id.bustype, dev->id.vendor, dev->id.product, dev->id.version);

	seq_printf(seq, "N: Name=\"%s\"\n", dev->name ? dev->name : "");
	seq_printf(seq, "P: Phys=%s\n", dev->phys ? dev->phys : "");
	seq_printf(seq, "S: Sysfs=%s\n", path ? path : "");
	seq_printf(seq, "U: Uniq=%s\n", dev->uniq ? dev->uniq : "");
	seq_puts(seq, "H: Handlers=");

	list_for_each_entry(handle, &dev->h_list, d_node)
		seq_printf(seq, "%s ", handle->name);
	seq_putc(seq, '\n');

	input_seq_print_bitmap(seq, "PROP", dev->propbit, INPUT_PROP_MAX);

	input_seq_print_bitmap(seq, "EV", dev->evbit, EV_MAX);
	if (test_bit(EV_KEY, dev->evbit))
		input_seq_print_bitmap(seq, "KEY", dev->keybit, KEY_MAX);
	if (test_bit(EV_REL, dev->evbit))
		input_seq_print_bitmap(seq, "REL", dev->relbit, REL_MAX);
	if (test_bit(EV_ABS, dev->evbit))
		input_seq_print_bitmap(seq, "ABS", dev->absbit, ABS_MAX);
	if (test_bit(EV_MSC, dev->evbit))
		input_seq_print_bitmap(seq, "MSC", dev->mscbit, MSC_MAX);
	if (test_bit(EV_LED, dev->evbit))
		input_seq_print_bitmap(seq, "LED", dev->ledbit, LED_MAX);
	if (test_bit(EV_SND, dev->evbit))
		input_seq_print_bitmap(seq, "SND", dev->sndbit, SND_MAX);
	if (test_bit(EV_FF, dev->evbit))
		input_seq_print_bitmap(seq, "FF", dev->ffbit, FF_MAX);
	if (test_bit(EV_SW, dev->evbit))
		input_seq_print_bitmap(seq, "SW", dev->swbit, SW_MAX);

	seq_putc(seq, '\n');

	kfree(path);
	return 0;
}