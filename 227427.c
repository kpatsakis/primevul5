void __hci_req_pause_adv_instances(struct hci_request *req)
{
	bt_dev_dbg(req->hdev, "Pausing advertising instances");

	/* Call to disable any advertisements active on the controller.
	 * This will succeed even if no advertisements are configured.
	 */
	__hci_req_disable_advertising(req);

	/* If we are using software rotation, pause the loop */
	if (!ext_adv_capable(req->hdev))
		cancel_adv_timeout(req->hdev);
}