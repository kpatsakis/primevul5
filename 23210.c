static int __init efi_load_efivars(void)
{
	struct platform_device *pdev;

	if (!efi_enabled(EFI_RUNTIME_SERVICES))
		return 0;

	pdev = platform_device_register_simple("efivars", 0, NULL, 0);
	return PTR_ERR_OR_ZERO(pdev);
}