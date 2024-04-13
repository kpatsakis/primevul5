static int __init setup_acpi_sci(char *s)
{
	if (!s)
		return -EINVAL;
	if (!strcmp(s, "edge"))
		acpi_sci_flags.trigger = 1;
	else if (!strcmp(s, "level"))
		acpi_sci_flags.trigger = 3;
	else if (!strcmp(s, "high"))
		acpi_sci_flags.polarity = 1;
	else if (!strcmp(s, "low"))
		acpi_sci_flags.polarity = 3;
	else
		return -EINVAL;
	return 0;
}