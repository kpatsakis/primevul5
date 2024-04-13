 * iscsi_bsg_host_dispatch - Dispatch command to LLD.
 * @job: bsg job to be processed
 */
static int iscsi_bsg_host_dispatch(struct bsg_job *job)
{
	struct Scsi_Host *shost = iscsi_job_to_shost(job);
	struct iscsi_bsg_request *req = job->request;
	struct iscsi_bsg_reply *reply = job->reply;
	struct iscsi_internal *i = to_iscsi_internal(shost->transportt);
	int cmdlen = sizeof(uint32_t);	/* start with length of msgcode */
	int ret;

	/* check if we have the msgcode value at least */
	if (job->request_len < sizeof(uint32_t)) {
		ret = -ENOMSG;
		goto fail_host_msg;
	}

	/* Validate the host command */
	switch (req->msgcode) {
	case ISCSI_BSG_HST_VENDOR:
		cmdlen += sizeof(struct iscsi_bsg_host_vendor);
		if ((shost->hostt->vendor_id == 0L) ||
		    (req->rqst_data.h_vendor.vendor_id !=
			shost->hostt->vendor_id)) {
			ret = -ESRCH;
			goto fail_host_msg;
		}
		break;
	default:
		ret = -EBADR;
		goto fail_host_msg;
	}

	/* check if we really have all the request data needed */
	if (job->request_len < cmdlen) {
		ret = -ENOMSG;
		goto fail_host_msg;
	}

	ret = i->iscsi_transport->bsg_request(job);
	if (!ret)
		return 0;

fail_host_msg:
	/* return the errno failure code as the only status */
	BUG_ON(job->reply_len < sizeof(uint32_t));
	reply->reply_payload_rcv_len = 0;
	reply->result = ret;
	job->reply_len = sizeof(uint32_t);