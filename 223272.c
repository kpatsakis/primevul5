static ssize_t use_zero_page_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return single_hugepage_flag_show(kobj, attr, buf,
				TRANSPARENT_HUGEPAGE_USE_ZERO_PAGE_FLAG);
}