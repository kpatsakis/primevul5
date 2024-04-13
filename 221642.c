static ssize_t itlb_multihit_show_state(char *buf)
{
	if (itlb_multihit_kvm_mitigation)
		return sprintf(buf, "KVM: Mitigation: Split huge pages\n");
	else
		return sprintf(buf, "KVM: Vulnerable\n");
}