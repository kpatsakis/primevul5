static ssize_t defrag_show(struct kobject *kobj,
			   struct kobj_attribute *attr, char *buf)
{
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_DIRECT_FLAG, &transparent_hugepage_flags))
		return sprintf(buf, "[always] defer defer+madvise madvise never\n");
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_KSWAPD_FLAG, &transparent_hugepage_flags))
		return sprintf(buf, "always [defer] defer+madvise madvise never\n");
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_KSWAPD_OR_MADV_FLAG, &transparent_hugepage_flags))
		return sprintf(buf, "always defer [defer+madvise] madvise never\n");
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_REQ_MADV_FLAG, &transparent_hugepage_flags))
		return sprintf(buf, "always defer defer+madvise [madvise] never\n");
	return sprintf(buf, "always defer defer+madvise madvise [never]\n");
}