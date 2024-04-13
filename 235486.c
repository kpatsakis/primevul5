void ecall_enclave_ra_close(sgx_ra_context_t context) {
  try {
    enclave_ra_close(context);
  } catch (const std::runtime_error &e) {
    ocall_throw(e.what());
  }
}