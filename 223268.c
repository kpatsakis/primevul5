static ssize_t enabled_store(struct kobject *kobj,
			     struct kobj_attribute *attr,
			     const char *buf, size_t count)
{
	ssize_t ret = count;

	if (sysfs_streq(buf, "always")) {
		clear_bit(TRANSPARENT_HUGEPAGE_REQ_MADV_FLAG, &transparent_hugepage_flags);
		set_bit(TRANSPARENT_HUGEPAGE_FLAG, &transparent_hugepage_flags);
	} else if (sysfs_streq(buf, "madvise")) {
		clear_bit(TRANSPARENT_HUGEPAGE_FLAG, &transparent_hugepage_flags);
		set_bit(TRANSPARENT_HUGEPAGE_REQ_MADV_FLAG, &transparent_hugepage_flags);
	} else if (sysfs_streq(buf, "never")) {
		clear_bit(TRANSPARENT_HUGEPAGE_FLAG, &transparent_hugepage_flags);
		clear_bit(TRANSPARENT_HUGEPAGE_REQ_MADV_FLAG, &transparent_hugepage_flags);
	} else
		ret = -EINVAL;

	if (ret > 0) {
		int err = start_stop_khugepaged();
		if (err)
			ret = err;
	}
	return ret;
}