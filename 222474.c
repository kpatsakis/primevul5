long kvm_arch_vcpu_ioctl(struct file *filp,
			 unsigned int ioctl, unsigned long arg)
{
	struct kvm_vcpu *vcpu = filp->private_data;
	void __user *argp = (void __user *)arg;
	int idx;
	long r;
	u16 rc, rrc;

	vcpu_load(vcpu);

	switch (ioctl) {
	case KVM_S390_STORE_STATUS:
		idx = srcu_read_lock(&vcpu->kvm->srcu);
		r = kvm_s390_store_status_unloaded(vcpu, arg);
		srcu_read_unlock(&vcpu->kvm->srcu, idx);
		break;
	case KVM_S390_SET_INITIAL_PSW: {
		psw_t psw;

		r = -EFAULT;
		if (copy_from_user(&psw, argp, sizeof(psw)))
			break;
		r = kvm_arch_vcpu_ioctl_set_initial_psw(vcpu, psw);
		break;
	}
	case KVM_S390_CLEAR_RESET:
		r = 0;
		kvm_arch_vcpu_ioctl_clear_reset(vcpu);
		if (kvm_s390_pv_cpu_is_protected(vcpu)) {
			r = uv_cmd_nodata(kvm_s390_pv_cpu_get_handle(vcpu),
					  UVC_CMD_CPU_RESET_CLEAR, &rc, &rrc);
			VCPU_EVENT(vcpu, 3, "PROTVIRT RESET CLEAR VCPU: rc %x rrc %x",
				   rc, rrc);
		}
		break;
	case KVM_S390_INITIAL_RESET:
		r = 0;
		kvm_arch_vcpu_ioctl_initial_reset(vcpu);
		if (kvm_s390_pv_cpu_is_protected(vcpu)) {
			r = uv_cmd_nodata(kvm_s390_pv_cpu_get_handle(vcpu),
					  UVC_CMD_CPU_RESET_INITIAL,
					  &rc, &rrc);
			VCPU_EVENT(vcpu, 3, "PROTVIRT RESET INITIAL VCPU: rc %x rrc %x",
				   rc, rrc);
		}
		break;
	case KVM_S390_NORMAL_RESET:
		r = 0;
		kvm_arch_vcpu_ioctl_normal_reset(vcpu);
		if (kvm_s390_pv_cpu_is_protected(vcpu)) {
			r = uv_cmd_nodata(kvm_s390_pv_cpu_get_handle(vcpu),
					  UVC_CMD_CPU_RESET, &rc, &rrc);
			VCPU_EVENT(vcpu, 3, "PROTVIRT RESET NORMAL VCPU: rc %x rrc %x",
				   rc, rrc);
		}
		break;
	case KVM_SET_ONE_REG:
	case KVM_GET_ONE_REG: {
		struct kvm_one_reg reg;
		r = -EINVAL;
		if (kvm_s390_pv_cpu_is_protected(vcpu))
			break;
		r = -EFAULT;
		if (copy_from_user(&reg, argp, sizeof(reg)))
			break;
		if (ioctl == KVM_SET_ONE_REG)
			r = kvm_arch_vcpu_ioctl_set_one_reg(vcpu, &reg);
		else
			r = kvm_arch_vcpu_ioctl_get_one_reg(vcpu, &reg);
		break;
	}
#ifdef CONFIG_KVM_S390_UCONTROL
	case KVM_S390_UCAS_MAP: {
		struct kvm_s390_ucas_mapping ucasmap;

		if (copy_from_user(&ucasmap, argp, sizeof(ucasmap))) {
			r = -EFAULT;
			break;
		}

		if (!kvm_is_ucontrol(vcpu->kvm)) {
			r = -EINVAL;
			break;
		}

		r = gmap_map_segment(vcpu->arch.gmap, ucasmap.user_addr,
				     ucasmap.vcpu_addr, ucasmap.length);
		break;
	}
	case KVM_S390_UCAS_UNMAP: {
		struct kvm_s390_ucas_mapping ucasmap;

		if (copy_from_user(&ucasmap, argp, sizeof(ucasmap))) {
			r = -EFAULT;
			break;
		}

		if (!kvm_is_ucontrol(vcpu->kvm)) {
			r = -EINVAL;
			break;
		}

		r = gmap_unmap_segment(vcpu->arch.gmap, ucasmap.vcpu_addr,
			ucasmap.length);
		break;
	}
#endif
	case KVM_S390_VCPU_FAULT: {
		r = gmap_fault(vcpu->arch.gmap, arg, 0);
		break;
	}
	case KVM_ENABLE_CAP:
	{
		struct kvm_enable_cap cap;
		r = -EFAULT;
		if (copy_from_user(&cap, argp, sizeof(cap)))
			break;
		r = kvm_vcpu_ioctl_enable_cap(vcpu, &cap);
		break;
	}
	case KVM_S390_MEM_OP: {
		struct kvm_s390_mem_op mem_op;

		if (copy_from_user(&mem_op, argp, sizeof(mem_op)) == 0)
			r = kvm_s390_guest_memsida_op(vcpu, &mem_op);
		else
			r = -EFAULT;
		break;
	}
	case KVM_S390_SET_IRQ_STATE: {
		struct kvm_s390_irq_state irq_state;

		r = -EFAULT;
		if (copy_from_user(&irq_state, argp, sizeof(irq_state)))
			break;
		if (irq_state.len > VCPU_IRQS_MAX_BUF ||
		    irq_state.len == 0 ||
		    irq_state.len % sizeof(struct kvm_s390_irq) > 0) {
			r = -EINVAL;
			break;
		}
		/* do not use irq_state.flags, it will break old QEMUs */
		r = kvm_s390_set_irq_state(vcpu,
					   (void __user *) irq_state.buf,
					   irq_state.len);
		break;
	}
	case KVM_S390_GET_IRQ_STATE: {
		struct kvm_s390_irq_state irq_state;

		r = -EFAULT;
		if (copy_from_user(&irq_state, argp, sizeof(irq_state)))
			break;
		if (irq_state.len == 0) {
			r = -EINVAL;
			break;
		}
		/* do not use irq_state.flags, it will break old QEMUs */
		r = kvm_s390_get_irq_state(vcpu,
					   (__u8 __user *)  irq_state.buf,
					   irq_state.len);
		break;
	}
	default:
		r = -ENOTTY;
	}

	vcpu_put(vcpu);
	return r;
}