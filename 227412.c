static int powered_update_hci(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;
	u8 link_sec;

	hci_dev_lock(hdev);

	if (hci_dev_test_flag(hdev, HCI_SSP_ENABLED) &&
	    !lmp_host_ssp_capable(hdev)) {
		u8 mode = 0x01;

		hci_req_add(req, HCI_OP_WRITE_SSP_MODE, sizeof(mode), &mode);

		if (bredr_sc_enabled(hdev) && !lmp_host_sc_capable(hdev)) {
			u8 support = 0x01;

			hci_req_add(req, HCI_OP_WRITE_SC_SUPPORT,
				    sizeof(support), &support);
		}
	}

	if (hci_dev_test_flag(hdev, HCI_LE_ENABLED) &&
	    lmp_bredr_capable(hdev)) {
		struct hci_cp_write_le_host_supported cp;

		cp.le = 0x01;
		cp.simul = 0x00;

		/* Check first if we already have the right
		 * host state (host features set)
		 */
		if (cp.le != lmp_host_le_capable(hdev) ||
		    cp.simul != lmp_host_le_br_capable(hdev))
			hci_req_add(req, HCI_OP_WRITE_LE_HOST_SUPPORTED,
				    sizeof(cp), &cp);
	}

	if (hci_dev_test_flag(hdev, HCI_LE_ENABLED)) {
		/* Make sure the controller has a good default for
		 * advertising data. This also applies to the case
		 * where BR/EDR was toggled during the AUTO_OFF phase.
		 */
		if (hci_dev_test_flag(hdev, HCI_ADVERTISING) ||
		    list_empty(&hdev->adv_instances)) {
			int err;

			if (ext_adv_capable(hdev)) {
				err = __hci_req_setup_ext_adv_instance(req,
								       0x00);
				if (!err)
					__hci_req_update_scan_rsp_data(req,
								       0x00);
			} else {
				err = 0;
				__hci_req_update_adv_data(req, 0x00);
				__hci_req_update_scan_rsp_data(req, 0x00);
			}

			if (hci_dev_test_flag(hdev, HCI_ADVERTISING)) {
				if (!ext_adv_capable(hdev))
					__hci_req_enable_advertising(req);
				else if (!err)
					__hci_req_enable_ext_advertising(req,
									 0x00);
			}
		} else if (!list_empty(&hdev->adv_instances)) {
			struct adv_info *adv_instance;

			adv_instance = list_first_entry(&hdev->adv_instances,
							struct adv_info, list);
			__hci_req_schedule_adv_instance(req,
							adv_instance->instance,
							true);
		}
	}

	link_sec = hci_dev_test_flag(hdev, HCI_LINK_SECURITY);
	if (link_sec != test_bit(HCI_AUTH, &hdev->flags))
		hci_req_add(req, HCI_OP_WRITE_AUTH_ENABLE,
			    sizeof(link_sec), &link_sec);

	if (lmp_bredr_capable(hdev)) {
		if (hci_dev_test_flag(hdev, HCI_FAST_CONNECTABLE))
			__hci_req_write_fast_connectable(req, true);
		else
			__hci_req_write_fast_connectable(req, false);
		__hci_req_update_scan(req);
		__hci_req_update_class(req);
		__hci_req_update_name(req);
		__hci_req_update_eir(req);
	}

	hci_dev_unlock(hdev);
	return 0;
}