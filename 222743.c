int kvm_arch_hardware_setup(void)
{
	gmap_notifier.notifier_call = kvm_gmap_notifier;
	gmap_register_pte_notifier(&gmap_notifier);
	vsie_gmap_notifier.notifier_call = kvm_s390_vsie_gmap_notifier;
	gmap_register_pte_notifier(&vsie_gmap_notifier);
	atomic_notifier_chain_register(&s390_epoch_delta_notifier,
				       &kvm_clock_notifier);
	return 0;
}