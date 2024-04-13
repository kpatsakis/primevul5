 * This function can be used by drives like qla4xxx to report to the scsi
 * layer when the scans it kicked off at module load time are done.
 */
int iscsi_scan_finished(struct Scsi_Host *shost, unsigned long time)
{
	struct iscsi_cls_host *ihost = shost->shost_data;
	/*
	 * qla4xxx will have kicked off some session unblocks before calling
	 * scsi_scan_host, so just wait for them to complete.