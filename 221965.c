static int synic_deliver_msg(struct kvm_vcpu_hv_synic *synic, u32 sint,
			     struct hv_message *src_msg, bool no_retry)
{
	struct kvm_vcpu *vcpu = hv_synic_to_vcpu(synic);
	int msg_off = offsetof(struct hv_message_page, sint_message[sint]);
	gfn_t msg_page_gfn;
	struct hv_message_header hv_hdr;
	int r;

	if (!(synic->msg_page & HV_SYNIC_SIMP_ENABLE))
		return -ENOENT;

	msg_page_gfn = synic->msg_page >> PAGE_SHIFT;

	/*
	 * Strictly following the spec-mandated ordering would assume setting
	 * .msg_pending before checking .message_type.  However, this function
	 * is only called in vcpu context so the entire update is atomic from
	 * guest POV and thus the exact order here doesn't matter.
	 */
	r = kvm_vcpu_read_guest_page(vcpu, msg_page_gfn, &hv_hdr.message_type,
				     msg_off + offsetof(struct hv_message,
							header.message_type),
				     sizeof(hv_hdr.message_type));
	if (r < 0)
		return r;

	if (hv_hdr.message_type != HVMSG_NONE) {
		if (no_retry)
			return 0;

		hv_hdr.message_flags.msg_pending = 1;
		r = kvm_vcpu_write_guest_page(vcpu, msg_page_gfn,
					      &hv_hdr.message_flags,
					      msg_off +
					      offsetof(struct hv_message,
						       header.message_flags),
					      sizeof(hv_hdr.message_flags));
		if (r < 0)
			return r;
		return -EAGAIN;
	}

	r = kvm_vcpu_write_guest_page(vcpu, msg_page_gfn, src_msg, msg_off,
				      sizeof(src_msg->header) +
				      src_msg->header.payload_size);
	if (r < 0)
		return r;

	r = synic_set_irq(synic, sint);
	if (r < 0)
		return r;
	if (r == 0)
		return -EFAULT;
	return 0;
}