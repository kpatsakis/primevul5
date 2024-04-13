static ssize_t store_rxbuf(struct device *dev,
			   struct device_attribute *attr,
			   const char *buf, size_t len)
{
	char *endp;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	simple_strtoul(buf, &endp, 0);
	if (endp == buf)
		return -EBADMSG;

	/* rxbuf_min and rxbuf_max are no longer configurable. */

	return len;
}