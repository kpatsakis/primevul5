static int check_stack_read(struct bpf_verifier_env *env,
			    struct bpf_func_state *reg_state /* func where register points to */,
			    int off, int size, int value_regno)
{
	struct bpf_verifier_state *vstate = env->cur_state;
	struct bpf_func_state *state = vstate->frame[vstate->curframe];
	int i, slot = -off - 1, spi = slot / BPF_REG_SIZE;
	struct bpf_reg_state *reg;
	u8 *stype;

	if (reg_state->allocated_stack <= slot) {
		verbose(env, "invalid read from stack off %d+0 size %d\n",
			off, size);
		return -EACCES;
	}
	stype = reg_state->stack[spi].slot_type;
	reg = &reg_state->stack[spi].spilled_ptr;

	if (stype[0] == STACK_SPILL) {
		if (size != BPF_REG_SIZE) {
			if (reg->type != SCALAR_VALUE) {
				verbose_linfo(env, env->insn_idx, "; ");
				verbose(env, "invalid size of register fill\n");
				return -EACCES;
			}
			if (value_regno >= 0) {
				mark_reg_unknown(env, state->regs, value_regno);
				state->regs[value_regno].live |= REG_LIVE_WRITTEN;
			}
			mark_reg_read(env, reg, reg->parent, REG_LIVE_READ64);
			return 0;
		}
		for (i = 1; i < BPF_REG_SIZE; i++) {
			if (stype[(slot - i) % BPF_REG_SIZE] != STACK_SPILL) {
				verbose(env, "corrupted spill memory\n");
				return -EACCES;
			}
		}

		if (value_regno >= 0) {
			/* restore register state from stack */
			state->regs[value_regno] = *reg;
			/* mark reg as written since spilled pointer state likely
			 * has its liveness marks cleared by is_state_visited()
			 * which resets stack/reg liveness for state transitions
			 */
			state->regs[value_regno].live |= REG_LIVE_WRITTEN;
		} else if (__is_pointer_value(env->allow_ptr_leaks, reg)) {
			/* If value_regno==-1, the caller is asking us whether
			 * it is acceptable to use this value as a SCALAR_VALUE
			 * (e.g. for XADD).
			 * We must not allow unprivileged callers to do that
			 * with spilled pointers.
			 */
			verbose(env, "leaking pointer from stack off %d\n",
				off);
			return -EACCES;
		}
		mark_reg_read(env, reg, reg->parent, REG_LIVE_READ64);
	} else {
		int zeros = 0;

		for (i = 0; i < size; i++) {
			if (stype[(slot - i) % BPF_REG_SIZE] == STACK_MISC)
				continue;
			if (stype[(slot - i) % BPF_REG_SIZE] == STACK_ZERO) {
				zeros++;
				continue;
			}
			verbose(env, "invalid read from stack off %d+%d size %d\n",
				off, i, size);
			return -EACCES;
		}
		mark_reg_read(env, reg, reg->parent, REG_LIVE_READ64);
		if (value_regno >= 0) {
			if (zeros == size) {
				/* any size read into register is zero extended,
				 * so the whole register == const_zero
				 */
				__mark_reg_const_zero(&state->regs[value_regno]);
				/* backtracking doesn't support STACK_ZERO yet,
				 * so mark it precise here, so that later
				 * backtracking can stop here.
				 * Backtracking may not need this if this register
				 * doesn't participate in pointer adjustment.
				 * Forward propagation of precise flag is not
				 * necessary either. This mark is only to stop
				 * backtracking. Any register that contributed
				 * to const 0 was marked precise before spill.
				 */
				state->regs[value_regno].precise = true;
			} else {
				/* have read misc data from the stack */
				mark_reg_unknown(env, state->regs, value_regno);
			}
			state->regs[value_regno].live |= REG_LIVE_WRITTEN;
		}
	}
	return 0;
}