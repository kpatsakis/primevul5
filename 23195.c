static int __init efisubsys_init(void)
{
	int error;

	if (!efi_enabled(EFI_BOOT))
		return 0;

	/*
	 * Since we process only one efi_runtime_service() at a time, an
	 * ordered workqueue (which creates only one execution context)
	 * should suffice all our needs.
	 */
	efi_rts_wq = alloc_ordered_workqueue("efi_rts_wq", 0);
	if (!efi_rts_wq) {
		pr_err("Creating efi_rts_wq failed, EFI runtime services disabled.\n");
		clear_bit(EFI_RUNTIME_SERVICES, &efi.flags);
		return 0;
	}

	/* We register the efi directory at /sys/firmware/efi */
	efi_kobj = kobject_create_and_add("efi", firmware_kobj);
	if (!efi_kobj) {
		pr_err("efi: Firmware registration failed.\n");
		return -ENOMEM;
	}

	error = generic_ops_register();
	if (error)
		goto err_put;

	if (efi_enabled(EFI_RUNTIME_SERVICES))
		efivar_ssdt_load();

	error = sysfs_create_group(efi_kobj, &efi_subsys_attr_group);
	if (error) {
		pr_err("efi: Sysfs attribute export failed with error %d.\n",
		       error);
		goto err_unregister;
	}

	error = efi_runtime_map_init(efi_kobj);
	if (error)
		goto err_remove_group;

	/* and the standard mountpoint for efivarfs */
	error = sysfs_create_mount_point(efi_kobj, "efivars");
	if (error) {
		pr_err("efivars: Subsystem registration failed.\n");
		goto err_remove_group;
	}

	return 0;

err_remove_group:
	sysfs_remove_group(efi_kobj, &efi_subsys_attr_group);
err_unregister:
	generic_ops_unregister();
err_put:
	kobject_put(efi_kobj);
	return error;
}