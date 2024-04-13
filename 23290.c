static ssize_t show_rxbuf(struct device *dev,
			  struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", NET_RX_RING_SIZE);
}