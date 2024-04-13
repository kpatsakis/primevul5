void kernel_halt(void)
{
	kernel_shutdown_prepare(SYSTEM_HALT);
	printk(KERN_EMERG "System halted.\n");
	machine_halt();
}