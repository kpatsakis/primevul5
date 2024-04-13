void kernel_power_off(void)
{
	kernel_shutdown_prepare(SYSTEM_POWER_OFF);
	printk(KERN_EMERG "Power down.\n");
	machine_power_off();
}