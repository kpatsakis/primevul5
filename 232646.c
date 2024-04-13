static int get_pop_bytes(bool *callee_regs_used)
{
	int bytes = 0;

	if (callee_regs_used[3])
		bytes += 2;
	if (callee_regs_used[2])
		bytes += 2;
	if (callee_regs_used[1])
		bytes += 2;
	if (callee_regs_used[0])
		bytes += 1;

	return bytes;
}