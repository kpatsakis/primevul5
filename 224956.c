 * @ihost: iscsi_cls_host adding the structures to
 */
static int
iscsi_bsg_host_add(struct Scsi_Host *shost, struct iscsi_cls_host *ihost)
{
	struct device *dev = &shost->shost_gendev;
	struct iscsi_internal *i = to_iscsi_internal(shost->transportt);
	struct request_queue *q;
	char bsg_name[20];

	if (!i->iscsi_transport->bsg_request)
		return -ENOTSUPP;

	snprintf(bsg_name, sizeof(bsg_name), "iscsi_host%d", shost->host_no);
	q = bsg_setup_queue(dev, bsg_name, iscsi_bsg_host_dispatch, NULL, 0);
	if (IS_ERR(q)) {
		shost_printk(KERN_ERR, shost, "bsg interface failed to "
			     "initialize - no request queue\n");
		return PTR_ERR(q);
	}
	__scsi_init_queue(shost, q);
