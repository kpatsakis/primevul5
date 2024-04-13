static int check_btf_func(struct bpf_verifier_env *env,
			  const union bpf_attr *attr,
			  union bpf_attr __user *uattr)
{
	u32 i, nfuncs, urec_size, min_size;
	u32 krec_size = sizeof(struct bpf_func_info);
	struct bpf_func_info *krecord;
	struct bpf_func_info_aux *info_aux = NULL;
	const struct btf_type *type;
	struct bpf_prog *prog;
	const struct btf *btf;
	void __user *urecord;
	u32 prev_offset = 0;
	int ret = -ENOMEM;

	nfuncs = attr->func_info_cnt;
	if (!nfuncs)
		return 0;

	if (nfuncs != env->subprog_cnt) {
		verbose(env, "number of funcs in func_info doesn't match number of subprogs\n");
		return -EINVAL;
	}

	urec_size = attr->func_info_rec_size;
	if (urec_size < MIN_BPF_FUNCINFO_SIZE ||
	    urec_size > MAX_FUNCINFO_REC_SIZE ||
	    urec_size % sizeof(u32)) {
		verbose(env, "invalid func info rec size %u\n", urec_size);
		return -EINVAL;
	}

	prog = env->prog;
	btf = prog->aux->btf;

	urecord = u64_to_user_ptr(attr->func_info);
	min_size = min_t(u32, krec_size, urec_size);

	krecord = kvcalloc(nfuncs, krec_size, GFP_KERNEL | __GFP_NOWARN);
	if (!krecord)
		return -ENOMEM;
	info_aux = kcalloc(nfuncs, sizeof(*info_aux), GFP_KERNEL | __GFP_NOWARN);
	if (!info_aux)
		goto err_free;

	for (i = 0; i < nfuncs; i++) {
		ret = bpf_check_uarg_tail_zero(urecord, krec_size, urec_size);
		if (ret) {
			if (ret == -E2BIG) {
				verbose(env, "nonzero tailing record in func info");
				/* set the size kernel expects so loader can zero
				 * out the rest of the record.
				 */
				if (put_user(min_size, &uattr->func_info_rec_size))
					ret = -EFAULT;
			}
			goto err_free;
		}

		if (copy_from_user(&krecord[i], urecord, min_size)) {
			ret = -EFAULT;
			goto err_free;
		}

		/* check insn_off */
		if (i == 0) {
			if (krecord[i].insn_off) {
				verbose(env,
					"nonzero insn_off %u for the first func info record",
					krecord[i].insn_off);
				ret = -EINVAL;
				goto err_free;
			}
		} else if (krecord[i].insn_off <= prev_offset) {
			verbose(env,
				"same or smaller insn offset (%u) than previous func info record (%u)",
				krecord[i].insn_off, prev_offset);
			ret = -EINVAL;
			goto err_free;
		}

		if (env->subprog_info[i].start != krecord[i].insn_off) {
			verbose(env, "func_info BTF section doesn't match subprog layout in BPF program\n");
			ret = -EINVAL;
			goto err_free;
		}

		/* check type_id */
		type = btf_type_by_id(btf, krecord[i].type_id);
		if (!type || !btf_type_is_func(type)) {
			verbose(env, "invalid type id %d in func info",
				krecord[i].type_id);
			ret = -EINVAL;
			goto err_free;
		}
		info_aux[i].linkage = BTF_INFO_VLEN(type->info);
		prev_offset = krecord[i].insn_off;
		urecord += urec_size;
	}

	prog->aux->func_info = krecord;
	prog->aux->func_info_cnt = nfuncs;
	prog->aux->func_info_aux = info_aux;
	return 0;

err_free:
	kvfree(krecord);
	kfree(info_aux);
	return ret;
}