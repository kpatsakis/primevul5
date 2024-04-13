static int mwait_interception(struct vcpu_svm *svm)
{
	printk_once(KERN_WARNING "kvm: MWAIT instruction emulated as NOP!\n");
	return nop_interception(svm);
}