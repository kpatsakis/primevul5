static ssize_t debug_cow_show(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return single_hugepage_flag_show(kobj, attr, buf,
				TRANSPARENT_HUGEPAGE_DEBUG_COW_FLAG);
}