static void nested_prepare_vmcb_save(struct vcpu_svm *svm, struct vmcb *vmcb12)
{
	/* Load the nested guest state */
	svm->vmcb->save.es = vmcb12->save.es;
	svm->vmcb->save.cs = vmcb12->save.cs;
	svm->vmcb->save.ss = vmcb12->save.ss;
	svm->vmcb->save.ds = vmcb12->save.ds;
	svm->vmcb->save.gdtr = vmcb12->save.gdtr;
	svm->vmcb->save.idtr = vmcb12->save.idtr;
	kvm_set_rflags(&svm->vcpu, vmcb12->save.rflags);
	svm_set_efer(&svm->vcpu, vmcb12->save.efer);
	svm_set_cr0(&svm->vcpu, vmcb12->save.cr0);
	svm_set_cr4(&svm->vcpu, vmcb12->save.cr4);
	svm->vmcb->save.cr2 = svm->vcpu.arch.cr2 = vmcb12->save.cr2;
	kvm_rax_write(&svm->vcpu, vmcb12->save.rax);
	kvm_rsp_write(&svm->vcpu, vmcb12->save.rsp);
	kvm_rip_write(&svm->vcpu, vmcb12->save.rip);

	/* In case we don't even reach vcpu_run, the fields are not updated */
	svm->vmcb->save.rax = vmcb12->save.rax;
	svm->vmcb->save.rsp = vmcb12->save.rsp;
	svm->vmcb->save.rip = vmcb12->save.rip;
	svm->vmcb->save.dr7 = vmcb12->save.dr7;
	svm->vcpu.arch.dr6  = vmcb12->save.dr6;
	svm->vmcb->save.cpl = vmcb12->save.cpl;
}