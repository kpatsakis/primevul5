static int kvm_s390_apxa_installed(void)
{
	struct ap_config_info info;

	if (ap_instructions_available()) {
		if (ap_qci(&info) == 0)
			return info.apxa;
	}

	return 0;
}