sgx_status_t ecall_verify_att_result_mac(sgx_ra_context_t context, uint8_t* message,
                                         size_t message_size, uint8_t* mac,
                                         size_t mac_size) {
  try {
    return verify_att_result_mac(context, message, message_size, mac, mac_size);
  } catch (const std::runtime_error &e) {
    ocall_throw(e.what());
    return SGX_ERROR_UNEXPECTED;
  }
}