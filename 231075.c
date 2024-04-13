static void svm_set_rflags(struct kvm_vcpu *vcpu, unsigned long rflags)
{
       /*
        * Any change of EFLAGS.VM is accompained by a reload of SS
        * (caused by either a task switch or an inter-privilege IRET),
        * so we do not need to update the CPL here.
        */
	to_svm(vcpu)->vmcb->save.rflags = rflags;
}