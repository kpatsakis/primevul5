static int check_stack_write(struct bpf_verifier_env *env,
			     struct bpf_func_state *state, /* func where register points to */
			     int off, int size, int value_regno, int insn_idx)
{
	struct bpf_func_state *cur; /* state of the current function */
	int i, slot = -off - 1, spi = slot / BPF_REG_SIZE, err;
	u32 dst_reg = env->prog->insnsi[insn_idx].dst_reg;
	struct bpf_reg_state *reg = NULL;

	err = realloc_func_state(state, round_up(slot + 1, BPF_REG_SIZE),
				 state->acquired_refs, true);
	if (err)
		return err;
	/* caller checked that off % size == 0 and -MAX_BPF_STACK <= off < 0,
	 * so it's aligned access and [off, off + size) are within stack limits
	 */
	if (!env->allow_ptr_leaks &&
	    state->stack[spi].slot_type[0] == STACK_SPILL &&
	    size != BPF_REG_SIZE) {
		verbose(env, "attempt to corrupt spilled pointer on stack\n");
		return -EACCES;
	}

	cur = env->cur_state->frame[env->cur_state->curframe];
	if (value_regno >= 0)
		reg = &cur->regs[value_regno];

	if (reg && size == BPF_REG_SIZE && register_is_bounded(reg) &&
	    !register_is_null(reg) && env->bpf_capable) {
		if (dst_reg != BPF_REG_FP) {
			/* The backtracking logic can only recognize explicit
			 * stack slot address like [fp - 8]. Other spill of
			 * scalar via different register has to be conervative.
			 * Backtrack from here and mark all registers as precise
			 * that contributed into 'reg' being a constant.
			 */
			err = mark_chain_precision(env, value_regno);
			if (err)
				return err;
		}
		save_register_state(state, spi, reg);
	} else if (reg && is_spillable_regtype(reg->type)) {
		/* register containing pointer is being spilled into stack */
		if (size != BPF_REG_SIZE) {
			verbose_linfo(env, insn_idx, "; ");
			verbose(env, "invalid size of register spill\n");
			return -EACCES;
		}

		if (state != cur && reg->type == PTR_TO_STACK) {
			verbose(env, "cannot spill pointers to stack into stack frame of the caller\n");
			return -EINVAL;
		}

		if (!env->bypass_spec_v4) {
			bool sanitize = false;

			if (state->stack[spi].slot_type[0] == STACK_SPILL &&
			    register_is_const(&state->stack[spi].spilled_ptr))
				sanitize = true;
			for (i = 0; i < BPF_REG_SIZE; i++)
				if (state->stack[spi].slot_type[i] == STACK_MISC) {
					sanitize = true;
					break;
				}
			if (sanitize) {
				int *poff = &env->insn_aux_data[insn_idx].sanitize_stack_off;
				int soff = (-spi - 1) * BPF_REG_SIZE;

				/* detected reuse of integer stack slot with a pointer
				 * which means either llvm is reusing stack slot or
				 * an attacker is trying to exploit CVE-2018-3639
				 * (speculative store bypass)
				 * Have to sanitize that slot with preemptive
				 * store of zero.
				 */
				if (*poff && *poff != soff) {
					/* disallow programs where single insn stores
					 * into two different stack slots, since verifier
					 * cannot sanitize them
					 */
					verbose(env,
						"insn %d cannot access two stack slots fp%d and fp%d",
						insn_idx, *poff, soff);
					return -EINVAL;
				}
				*poff = soff;
			}
		}
		save_register_state(state, spi, reg);
	} else {
		u8 type = STACK_MISC;

		/* regular write of data into stack destroys any spilled ptr */
		state->stack[spi].spilled_ptr.type = NOT_INIT;
		/* Mark slots as STACK_MISC if they belonged to spilled ptr. */
		if (state->stack[spi].slot_type[0] == STACK_SPILL)
			for (i = 0; i < BPF_REG_SIZE; i++)
				state->stack[spi].slot_type[i] = STACK_MISC;

		/* only mark the slot as written if all 8 bytes were written
		 * otherwise read propagation may incorrectly stop too soon
		 * when stack slots are partially written.
		 * This heuristic means that read propagation will be
		 * conservative, since it will add reg_live_read marks
		 * to stack slots all the way to first state when programs
		 * writes+reads less than 8 bytes
		 */
		if (size == BPF_REG_SIZE)
			state->stack[spi].spilled_ptr.live |= REG_LIVE_WRITTEN;

		/* when we zero initialize stack slots mark them as such */
		if (reg && register_is_null(reg)) {
			/* backtracking doesn't work for STACK_ZERO yet. */
			err = mark_chain_precision(env, value_regno);
			if (err)
				return err;
			type = STACK_ZERO;
		}

		/* Mark slots affected by this stack write. */
		for (i = 0; i < size; i++)
			state->stack[spi].slot_type[(slot - i) % BPF_REG_SIZE] =
				type;
	}
	return 0;
}