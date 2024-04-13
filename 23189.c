static int __init efivar_ssdt_setup(char *str)
{
	int ret = security_locked_down(LOCKDOWN_ACPI_TABLES);

	if (ret)
		return ret;

	if (strlen(str) < sizeof(efivar_ssdt))
		memcpy(efivar_ssdt, str, strlen(str));
	else
		pr_warn("efivar_ssdt: name too long: %s\n", str);
	return 0;
}