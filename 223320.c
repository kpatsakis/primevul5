static ssize_t hpage_pmd_size_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", HPAGE_PMD_SIZE);
}