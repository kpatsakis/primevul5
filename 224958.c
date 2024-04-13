	return 0;
}

static int iscsi_user_scan(struct Scsi_Host *shost, uint channel,
			   uint id, u64 lun)
{
	struct iscsi_scan_data scan_data;

	scan_data.channel = channel;
	scan_data.id = id;
	scan_data.lun = lun;
	scan_data.rescan = SCSI_SCAN_MANUAL;
