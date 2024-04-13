static void kernel_kexec(void)
{
#ifdef CONFIG_KEXEC
	struct kimage *image;
	image = xchg(&kexec_image, NULL);
	if (!image)
		return;
	kernel_restart_prepare(NULL);
	printk(KERN_EMERG "Starting new kernel\n");
	machine_shutdown();
	machine_kexec(image);
#endif
}