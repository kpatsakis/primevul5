static bool svm_check_apicv_inhibit_reasons(ulong bit)
{
	ulong supported = BIT(APICV_INHIBIT_REASON_DISABLE) |
			  BIT(APICV_INHIBIT_REASON_HYPERV) |
			  BIT(APICV_INHIBIT_REASON_NESTED) |
			  BIT(APICV_INHIBIT_REASON_IRQWIN) |
			  BIT(APICV_INHIBIT_REASON_PIT_REINJ);

	return supported & BIT(bit);
}