static int __kvm_ucontrol_vcpu_init(struct kvm_vcpu *vcpu)
{
	vcpu->arch.gmap = gmap_create(current->mm, -1UL);
	if (!vcpu->arch.gmap)
		return -ENOMEM;
	vcpu->arch.gmap->private = vcpu->kvm;

	return 0;
}