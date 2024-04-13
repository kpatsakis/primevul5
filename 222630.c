void kvm_arch_hardware_unsetup(void)
{
	gmap_unregister_pte_notifier(&gmap_notifier);
	gmap_unregister_pte_notifier(&vsie_gmap_notifier);
	atomic_notifier_chain_unregister(&s390_epoch_delta_notifier,
					 &kvm_clock_notifier);
}