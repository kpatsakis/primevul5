static int __init text_mode(char *str)
{
	vgacon_text_mode_force = true;

	pr_warn("You have booted with nomodeset. This means your GPU drivers are DISABLED\n");
	pr_warn("Any video related functionality will be severely degraded, and you may not even be able to suspend the system properly\n");
	pr_warn("Unless you actually understand what nomodeset does, you should reboot without enabling it\n");

	return 1;
}