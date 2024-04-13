int query_sgx_status()
{
  sgx_device_status_t sgx_device_status;
  sgx_status_t sgx_ret = sgx_enable_device(&sgx_device_status);
  if (sgx_ret != SGX_SUCCESS) {
    printf("Failed to get SGX device status.\n");
    return -1;
  }
  else {
    switch (sgx_device_status) {
    case SGX_ENABLED:
      return 0;
    case SGX_DISABLED_REBOOT_REQUIRED:
      printf("SGX device has been enabled. Please reboot your machine.\n");
      return -1;
    case SGX_DISABLED_LEGACY_OS:
      printf("SGX device can't be enabled on an OS that doesn't support EFI interface.\n");
      return -1;
    case SGX_DISABLED:
      printf("SGX device not found.\n");
      return -1;
    default:
      printf("Unexpected error.\n");
      return -1;
    }
  }
}