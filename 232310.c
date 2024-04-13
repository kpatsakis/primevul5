static int check_pseudo_btf_id(struct bpf_verifier_env *env,
			       struct bpf_insn *insn,
			       struct bpf_insn_aux_data *aux)
{
	const struct btf_var_secinfo *vsi;
	const struct btf_type *datasec;
	const struct btf_type *t;
	const char *sym_name;
	bool percpu = false;
	u32 type, id = insn->imm;
	s32 datasec_id;
	u64 addr;
	int i;

	if (!btf_vmlinux) {
		verbose(env, "kernel is missing BTF, make sure CONFIG_DEBUG_INFO_BTF=y is specified in Kconfig.\n");
		return -EINVAL;
	}

	if (insn[1].imm != 0) {
		verbose(env, "reserved field (insn[1].imm) is used in pseudo_btf_id ldimm64 insn.\n");
		return -EINVAL;
	}

	t = btf_type_by_id(btf_vmlinux, id);
	if (!t) {
		verbose(env, "ldimm64 insn specifies invalid btf_id %d.\n", id);
		return -ENOENT;
	}

	if (!btf_type_is_var(t)) {
		verbose(env, "pseudo btf_id %d in ldimm64 isn't KIND_VAR.\n",
			id);
		return -EINVAL;
	}

	sym_name = btf_name_by_offset(btf_vmlinux, t->name_off);
	addr = kallsyms_lookup_name(sym_name);
	if (!addr) {
		verbose(env, "ldimm64 failed to find the address for kernel symbol '%s'.\n",
			sym_name);
		return -ENOENT;
	}

	datasec_id = btf_find_by_name_kind(btf_vmlinux, ".data..percpu",
					   BTF_KIND_DATASEC);
	if (datasec_id > 0) {
		datasec = btf_type_by_id(btf_vmlinux, datasec_id);
		for_each_vsi(i, datasec, vsi) {
			if (vsi->type == id) {
				percpu = true;
				break;
			}
		}
	}

	insn[0].imm = (u32)addr;
	insn[1].imm = addr >> 32;

	type = t->type;
	t = btf_type_skip_modifiers(btf_vmlinux, type, NULL);
	if (percpu) {
		aux->btf_var.reg_type = PTR_TO_PERCPU_BTF_ID;
		aux->btf_var.btf = btf_vmlinux;
		aux->btf_var.btf_id = type;
	} else if (!btf_type_is_struct(t)) {
		const struct btf_type *ret;
		const char *tname;
		u32 tsize;

		/* resolve the type size of ksym. */
		ret = btf_resolve_size(btf_vmlinux, t, &tsize);
		if (IS_ERR(ret)) {
			tname = btf_name_by_offset(btf_vmlinux, t->name_off);
			verbose(env, "ldimm64 unable to resolve the size of type '%s': %ld\n",
				tname, PTR_ERR(ret));
			return -EINVAL;
		}
		aux->btf_var.reg_type = PTR_TO_MEM;
		aux->btf_var.mem_size = tsize;
	} else {
		aux->btf_var.reg_type = PTR_TO_BTF_ID;
		aux->btf_var.btf = btf_vmlinux;
		aux->btf_var.btf_id = type;
	}
	return 0;
}