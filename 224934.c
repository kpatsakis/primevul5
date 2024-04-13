	return 0;
}

void iscsi_host_for_each_session(struct Scsi_Host *shost,
				 void (*fn)(struct iscsi_cls_session *))
{