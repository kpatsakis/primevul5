static int nfs4_map_errors(int err)
{
	if (err >= -1000)
		return err;
	switch (err) {
	case -NFS4ERR_RESOURCE:
	case -NFS4ERR_LAYOUTTRYLATER:
	case -NFS4ERR_RECALLCONFLICT:
		return -EREMOTEIO;
	case -NFS4ERR_WRONGSEC:
	case -NFS4ERR_WRONG_CRED:
		return -EPERM;
	case -NFS4ERR_BADOWNER:
	case -NFS4ERR_BADNAME:
		return -EINVAL;
	case -NFS4ERR_SHARE_DENIED:
		return -EACCES;
	case -NFS4ERR_MINOR_VERS_MISMATCH:
		return -EPROTONOSUPPORT;
	case -NFS4ERR_FILE_OPEN:
		return -EBUSY;
	default:
		dprintk("%s could not handle NFSv4 error %d\n",
				__func__, -err);
		break;
	}
	return -EIO;
}