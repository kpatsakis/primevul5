static int monitor_interception(struct vcpu_svm *svm)
{
	printk_once(KERN_WARNING "kvm: MONITOR instruction emulated as NOP!\n");
	return nop_interception(svm);
}