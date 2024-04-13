sgx_status_t ecall_enclave_init_ra(int b_pse, sgx_ra_context_t *p_context) {
  try {
    return enclave_init_ra(b_pse, p_context);
  } catch (const std::runtime_error &e) {
    ocall_throw(e.what());
    return SGX_ERROR_UNEXPECTED;
  }
}