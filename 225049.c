	return 0;
}

static int iscsi_remove_host(struct transport_container *tc,
			     struct device *dev, struct device *cdev)
{
	struct Scsi_Host *shost = dev_to_shost(dev);
	struct iscsi_cls_host *ihost = shost->shost_data;
