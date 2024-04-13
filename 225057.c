	return 0;
}

static int iscsi_setup_host(struct transport_container *tc, struct device *dev,
			    struct device *cdev)
{
	struct Scsi_Host *shost = dev_to_shost(dev);
	struct iscsi_cls_host *ihost = shost->shost_data;

	memset(ihost, 0, sizeof(*ihost));
	atomic_set(&ihost->nr_scans, 0);
	mutex_init(&ihost->mutex);

	iscsi_bsg_host_add(shost, ihost);
	/* ignore any bsg add error - we just can't do sgio */