void sgx_check_quiet(const char* message, sgx_status_t ret)
{
  if (ret != SGX_SUCCESS) {
    printf("%s failed\n", message);
    print_error_message(ret);
  }
}