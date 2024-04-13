sgx_status_t ecall_put_secret_data(sgx_ra_context_t context,
                                   uint8_t* p_secret,
                                   uint32_t secret_size,
                                   uint8_t* gcm_mac) {
  try {
    return put_secret_data(context, p_secret, secret_size, gcm_mac);
  } catch (const std::runtime_error &e) {
    ocall_throw(e.what());
    return SGX_ERROR_UNEXPECTED;
  }
}