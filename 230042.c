static bool nfs4_mode_match_open_stateid(struct nfs4_state *state,
		fmode_t fmode)
{
	switch(fmode & (FMODE_READ|FMODE_WRITE)) {
	case FMODE_READ|FMODE_WRITE:
		return state->n_rdwr != 0;
	case FMODE_WRITE:
		return state->n_wronly != 0;
	case FMODE_READ:
		return state->n_rdonly != 0;
	}
	WARN_ON_ONCE(1);
	return false;
}