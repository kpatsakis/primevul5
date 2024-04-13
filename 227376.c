static void __hci_req_resume_adv_instances(struct hci_request *req)
{
	struct adv_info *adv;

	bt_dev_dbg(req->hdev, "Resuming advertising instances");

	if (ext_adv_capable(req->hdev)) {
		/* Call for each tracked instance to be re-enabled */
		list_for_each_entry(adv, &req->hdev->adv_instances, list) {
			__hci_req_enable_ext_advertising(req,
							 adv->instance);
		}

	} else {
		/* Schedule for most recent instance to be restarted and begin
		 * the software rotation loop
		 */
		__hci_req_schedule_adv_instance(req,
						req->hdev->cur_adv_instance,
						true);
	}
}