void kernel_restart(char *cmd)
{
	kernel_restart_prepare(cmd);
	if (!cmd)
		printk(KERN_EMERG "Restarting system.\n");
	else
		printk(KERN_EMERG "Restarting system with command '%s'.\n", cmd);
	machine_restart(cmd);
}