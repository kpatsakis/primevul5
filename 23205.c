static ssize_t fw_platform_size_show(struct kobject *kobj,
				     struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", efi_enabled(EFI_64BIT) ? 64 : 32);
}