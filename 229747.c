static void nfs41_delegation_recover_stateid(struct nfs4_state *state)
{
	nfs4_stateid tmp;

	if (test_bit(NFS_DELEGATED_STATE, &state->flags) &&
	    nfs4_copy_delegation_stateid(state->inode, state->state,
				&tmp, NULL) &&
	    nfs4_stateid_match_other(&state->stateid, &tmp))
		nfs_state_set_delegation(state, &tmp, state->state);
	else
		nfs_state_clear_delegation(state);
}