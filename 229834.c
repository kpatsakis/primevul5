static int can_open_cached(struct nfs4_state *state, fmode_t mode,
		int open_mode, enum open_claim_type4 claim)
{
	int ret = 0;

	if (open_mode & (O_EXCL|O_TRUNC))
		goto out;
	switch (claim) {
	case NFS4_OPEN_CLAIM_NULL:
	case NFS4_OPEN_CLAIM_FH:
		goto out;
	default:
		break;
	}
	switch (mode & (FMODE_READ|FMODE_WRITE)) {
		case FMODE_READ:
			ret |= test_bit(NFS_O_RDONLY_STATE, &state->flags) != 0
				&& state->n_rdonly != 0;
			break;
		case FMODE_WRITE:
			ret |= test_bit(NFS_O_WRONLY_STATE, &state->flags) != 0
				&& state->n_wronly != 0;
			break;
		case FMODE_READ|FMODE_WRITE:
			ret |= test_bit(NFS_O_RDWR_STATE, &state->flags) != 0
				&& state->n_rdwr != 0;
	}
out:
	return ret;
}