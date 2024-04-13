void ocall_malloc(size_t size, uint8_t **ret) {
  unsafe_ocall_malloc(size, ret);

  // Guard against overwriting enclave memory
  assert(sgx_is_outside_enclave(*ret, size) == 1);
  sgx_lfence();
}