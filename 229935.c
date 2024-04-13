static int nfs4_sp4_select_mode(struct nfs_client *clp,
				 struct nfs41_state_protection *sp)
{
	static const u32 supported_enforce[NFS4_OP_MAP_NUM_WORDS] = {
		[1] = 1 << (OP_BIND_CONN_TO_SESSION - 32) |
		      1 << (OP_EXCHANGE_ID - 32) |
		      1 << (OP_CREATE_SESSION - 32) |
		      1 << (OP_DESTROY_SESSION - 32) |
		      1 << (OP_DESTROY_CLIENTID - 32)
	};
	unsigned long flags = 0;
	unsigned int i;
	int ret = 0;

	if (sp->how == SP4_MACH_CRED) {
		/* Print state protect result */
		dfprintk(MOUNT, "Server SP4_MACH_CRED support:\n");
		for (i = 0; i <= LAST_NFS4_OP; i++) {
			if (test_bit(i, sp->enforce.u.longs))
				dfprintk(MOUNT, "  enforce op %d\n", i);
			if (test_bit(i, sp->allow.u.longs))
				dfprintk(MOUNT, "  allow op %d\n", i);
		}

		/* make sure nothing is on enforce list that isn't supported */
		for (i = 0; i < NFS4_OP_MAP_NUM_WORDS; i++) {
			if (sp->enforce.u.words[i] & ~supported_enforce[i]) {
				dfprintk(MOUNT, "sp4_mach_cred: disabled\n");
				ret = -EINVAL;
				goto out;
			}
		}

		/*
		 * Minimal mode - state operations are allowed to use machine
		 * credential.  Note this already happens by default, so the
		 * client doesn't have to do anything more than the negotiation.
		 *
		 * NOTE: we don't care if EXCHANGE_ID is in the list -
		 *       we're already using the machine cred for exchange_id
		 *       and will never use a different cred.
		 */
		if (test_bit(OP_BIND_CONN_TO_SESSION, sp->enforce.u.longs) &&
		    test_bit(OP_CREATE_SESSION, sp->enforce.u.longs) &&
		    test_bit(OP_DESTROY_SESSION, sp->enforce.u.longs) &&
		    test_bit(OP_DESTROY_CLIENTID, sp->enforce.u.longs)) {
			dfprintk(MOUNT, "sp4_mach_cred:\n");
			dfprintk(MOUNT, "  minimal mode enabled\n");
			__set_bit(NFS_SP4_MACH_CRED_MINIMAL, &flags);
		} else {
			dfprintk(MOUNT, "sp4_mach_cred: disabled\n");
			ret = -EINVAL;
			goto out;
		}

		if (test_bit(OP_CLOSE, sp->allow.u.longs) &&
		    test_bit(OP_OPEN_DOWNGRADE, sp->allow.u.longs) &&
		    test_bit(OP_DELEGRETURN, sp->allow.u.longs) &&
		    test_bit(OP_LOCKU, sp->allow.u.longs)) {
			dfprintk(MOUNT, "  cleanup mode enabled\n");
			__set_bit(NFS_SP4_MACH_CRED_CLEANUP, &flags);
		}

		if (test_bit(OP_LAYOUTRETURN, sp->allow.u.longs)) {
			dfprintk(MOUNT, "  pnfs cleanup mode enabled\n");
			__set_bit(NFS_SP4_MACH_CRED_PNFS_CLEANUP, &flags);
		}

		if (test_bit(OP_SECINFO, sp->allow.u.longs) &&
		    test_bit(OP_SECINFO_NO_NAME, sp->allow.u.longs)) {
			dfprintk(MOUNT, "  secinfo mode enabled\n");
			__set_bit(NFS_SP4_MACH_CRED_SECINFO, &flags);
		}

		if (test_bit(OP_TEST_STATEID, sp->allow.u.longs) &&
		    test_bit(OP_FREE_STATEID, sp->allow.u.longs)) {
			dfprintk(MOUNT, "  stateid mode enabled\n");
			__set_bit(NFS_SP4_MACH_CRED_STATEID, &flags);
		}

		if (test_bit(OP_WRITE, sp->allow.u.longs)) {
			dfprintk(MOUNT, "  write mode enabled\n");
			__set_bit(NFS_SP4_MACH_CRED_WRITE, &flags);
		}

		if (test_bit(OP_COMMIT, sp->allow.u.longs)) {
			dfprintk(MOUNT, "  commit mode enabled\n");
			__set_bit(NFS_SP4_MACH_CRED_COMMIT, &flags);
		}
	}
out:
	clp->cl_sp4_flags = flags;
	return ret;
}