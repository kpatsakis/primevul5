static ssize_t dlpar_show(struct class *class, struct class_attribute *attr,
			  char *buf)
{
	return sprintf(buf, "%s\n", "memory,cpu");
}