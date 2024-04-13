static int kvm_s390_handle_pv(struct kvm *kvm, struct kvm_pv_cmd *cmd)
{
	int r = 0;
	u16 dummy;
	void __user *argp = (void __user *)cmd->data;

	switch (cmd->cmd) {
	case KVM_PV_ENABLE: {
		r = -EINVAL;
		if (kvm_s390_pv_is_protected(kvm))
			break;

		/*
		 *  FMT 4 SIE needs esca. As we never switch back to bsca from
		 *  esca, we need no cleanup in the error cases below
		 */
		r = sca_switch_to_extended(kvm);
		if (r)
			break;

		down_write(&current->mm->mmap_sem);
		r = gmap_mark_unmergeable();
		up_write(&current->mm->mmap_sem);
		if (r)
			break;

		r = kvm_s390_pv_init_vm(kvm, &cmd->rc, &cmd->rrc);
		if (r)
			break;

		r = kvm_s390_cpus_to_pv(kvm, &cmd->rc, &cmd->rrc);
		if (r)
			kvm_s390_pv_deinit_vm(kvm, &dummy, &dummy);

		/* we need to block service interrupts from now on */
		set_bit(IRQ_PEND_EXT_SERVICE, &kvm->arch.float_int.masked_irqs);
		break;
	}
	case KVM_PV_DISABLE: {
		r = -EINVAL;
		if (!kvm_s390_pv_is_protected(kvm))
			break;

		r = kvm_s390_cpus_from_pv(kvm, &cmd->rc, &cmd->rrc);
		/*
		 * If a CPU could not be destroyed, destroy VM will also fail.
		 * There is no point in trying to destroy it. Instead return
		 * the rc and rrc from the first CPU that failed destroying.
		 */
		if (r)
			break;
		r = kvm_s390_pv_deinit_vm(kvm, &cmd->rc, &cmd->rrc);

		/* no need to block service interrupts any more */
		clear_bit(IRQ_PEND_EXT_SERVICE, &kvm->arch.float_int.masked_irqs);
		break;
	}
	case KVM_PV_SET_SEC_PARMS: {
		struct kvm_s390_pv_sec_parm parms = {};
		void *hdr;

		r = -EINVAL;
		if (!kvm_s390_pv_is_protected(kvm))
			break;

		r = -EFAULT;
		if (copy_from_user(&parms, argp, sizeof(parms)))
			break;

		/* Currently restricted to 8KB */
		r = -EINVAL;
		if (parms.length > PAGE_SIZE * 2)
			break;

		r = -ENOMEM;
		hdr = vmalloc(parms.length);
		if (!hdr)
			break;

		r = -EFAULT;
		if (!copy_from_user(hdr, (void __user *)parms.origin,
				    parms.length))
			r = kvm_s390_pv_set_sec_parms(kvm, hdr, parms.length,
						      &cmd->rc, &cmd->rrc);

		vfree(hdr);
		break;
	}
	case KVM_PV_UNPACK: {
		struct kvm_s390_pv_unp unp = {};

		r = -EINVAL;
		if (!kvm_s390_pv_is_protected(kvm))
			break;

		r = -EFAULT;
		if (copy_from_user(&unp, argp, sizeof(unp)))
			break;

		r = kvm_s390_pv_unpack(kvm, unp.addr, unp.size, unp.tweak,
				       &cmd->rc, &cmd->rrc);
		break;
	}
	case KVM_PV_VERIFY: {
		r = -EINVAL;
		if (!kvm_s390_pv_is_protected(kvm))
			break;

		r = uv_cmd_nodata(kvm_s390_pv_get_handle(kvm),
				  UVC_CMD_VERIFY_IMG, &cmd->rc, &cmd->rrc);
		KVM_UV_EVENT(kvm, 3, "PROTVIRT VERIFY: rc %x rrc %x", cmd->rc,
			     cmd->rrc);
		break;
	}
	case KVM_PV_PREP_RESET: {
		r = -EINVAL;
		if (!kvm_s390_pv_is_protected(kvm))
			break;

		r = uv_cmd_nodata(kvm_s390_pv_get_handle(kvm),
				  UVC_CMD_PREPARE_RESET, &cmd->rc, &cmd->rrc);
		KVM_UV_EVENT(kvm, 3, "PROTVIRT PREP RESET: rc %x rrc %x",
			     cmd->rc, cmd->rrc);
		break;
	}
	case KVM_PV_UNSHARE_ALL: {
		r = -EINVAL;
		if (!kvm_s390_pv_is_protected(kvm))
			break;

		r = uv_cmd_nodata(kvm_s390_pv_get_handle(kvm),
				  UVC_CMD_SET_UNSHARE_ALL, &cmd->rc, &cmd->rrc);
		KVM_UV_EVENT(kvm, 3, "PROTVIRT UNSHARE: rc %x rrc %x",
			     cmd->rc, cmd->rrc);
		break;
	}
	default:
		r = -ENOTTY;
	}
	return r;
}