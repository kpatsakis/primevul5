static void nfs_send(void)
{
	debug("%s\n", __func__);

	switch (nfs_state) {
	case STATE_PRCLOOKUP_PROG_MOUNT_REQ:
		rpc_lookup_req(PROG_MOUNT, 1);
		break;
	case STATE_PRCLOOKUP_PROG_NFS_REQ:
		rpc_lookup_req(PROG_NFS, 2);
		break;
	case STATE_MOUNT_REQ:
		nfs_mount_req(nfs_path);
		break;
	case STATE_UMOUNT_REQ:
		nfs_umountall_req();
		break;
	case STATE_LOOKUP_REQ:
		nfs_lookup_req(nfs_filename);
		break;
	case STATE_READ_REQ:
		nfs_read_req(nfs_offset, NFS_READ_SIZE);
		break;
	case STATE_READLINK_REQ:
		nfs_readlink_req();
		break;
	}

	nfs_timer_start = get_time_ns();
}
