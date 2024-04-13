JNIEXPORT void JNICALL Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_RemoteAttestation3(
    JNIEnv *env, jobject obj,
    jlong eid,
    jbyteArray att_result_input) {

  (void)env;
  (void)obj;

#ifdef DEBUG
  printf("RemoteAttestation3 called\n");
#endif

  sgx_status_t status = SGX_SUCCESS;
  //uint32_t input_len = (uint32_t) env->GetArrayLength(att_result_input);
  jboolean if_copy = false;
  jbyte *ptr = env->GetByteArrayElements(att_result_input, &if_copy);

  ra_samp_response_header_t *att_result_full = (ra_samp_response_header_t *)(ptr);
  sample_ra_att_result_msg_t *att_result = (sample_ra_att_result_msg_t *) att_result_full->body;

#ifdef DEBUG
  printf("[RemoteAttestation3] att_result's size is %u\n", att_result_full->size);
#endif

  // Check the MAC using MK on the attestation result message.
  // The format of the attestation result message is ISV specific.
  // This is a simple form for demonstration. In a real product,
  // the ISV may want to communicate more information.
  int ret = 0;
  ret = ecall_verify_att_result_mac(eid,
                                    &status,
                                    context,
                                    (uint8_t*)&att_result->platform_info_blob,
                                    sizeof(ias_platform_info_blob_t),
                                    (uint8_t*)&att_result->mac,
                                    sizeof(sgx_mac_t));

  if((SGX_SUCCESS != ret) || (SGX_SUCCESS != status)) {
    fprintf(stdout, "\nError: INTEGRITY FAILED - attestation result message MK based cmac failed in [%s], status is %u", __FUNCTION__, (uint32_t) status);
    return ;
  }

  bool attestation_passed = true;
  // Check the attestation result for pass or fail.
  // Whether attestation passes or fails is a decision made by the ISV Server.
  // When the ISV server decides to trust the enclave, then it will return success.
  // When the ISV server decided to not trust the enclave, then it will return failure.
  if (0 != att_result_full->status[0] || 0 != att_result_full->status[1]) {
    fprintf(stdout, "\nError, attestation result message MK based cmac "
            "failed in [%s].", __FUNCTION__);
    attestation_passed = false;
  }

  // The attestation result message should contain a field for the Platform
  // Info Blob (PIB).  The PIB is returned by attestation server in the attestation report.
  // It is not returned in all cases, but when it is, the ISV app
  // should pass it to the blob analysis API called sgx_report_attestation_status()
  // along with the trust decision from the ISV server.
  // The ISV application will take action based on the update_info.
  // returned in update_info by the API.
  // This call is stubbed out for the sample.
  //
  // sgx_update_info_bit_t update_info;
  // ret = sgx_report_attestation_status(
  //     &p_att_result_msg_body->platform_info_blob,
  //     attestation_passed ? 0 : 1, &update_info);

  // Get the shared secret sent by the server using SK (if attestation
  // passed)
#ifdef DEBUG
  printf("[RemoteAttestation3] %u\n", attestation_passed);
#endif
  if (attestation_passed) {
    ret = ecall_put_secret_data(eid,
                                &status,
                                context,
                                att_result->secret.payload,
                                att_result->secret.payload_size,
                                att_result->secret.payload_tag);

    if((SGX_SUCCESS != ret)  || (SGX_SUCCESS != status)) {
      fprintf(stdout, "\nError, attestation result message secret "
              "using SK based AESGCM failed in [%s]. ret = "
              "0x%0x. status = 0x%0x", __FUNCTION__, ret,
              status);
      return ;
    }
  }

  fprintf(stdout, "\nSecret successfully received from server.");
  fprintf(stdout, "\nRemote attestation success!\n");

#ifdef DEBUG
  fprintf(stdout, "Destroying the key exchange context\n");
#endif
  ecall_enclave_ra_close(eid, context);
}