JNIEXPORT jbyteArray JNICALL Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_RemoteAttestation2(
    JNIEnv *env, jobject obj,
    jlong eid,
    jbyteArray msg2_input) {

  (void)env;
  (void)obj;

  int ret = 0;
  //sgx_ra_context_t context = INT_MAX;

  (void)ret;
  (void)eid;
  // Successfully sent msg1 and received a msg2 back.
  // Time now to check msg2.

  //uint32_t input_len = (uint32_t) env->GetArrayLength(msg2_input);
  jboolean if_copy = false;
  jbyte *ptr = env->GetByteArrayElements(msg2_input, &if_copy);
  sgx_ra_msg2_t* p_msg2_body = (sgx_ra_msg2_t*)(ptr);

#ifdef DEBUG
  printf("Printing p_msg2_body\n");
  PRINT_BYTE_ARRAY(stdout, p_msg2_body, sizeof(sgx_ra_msg2_t));
#endif

  uint32_t msg3_size = 0;
  sgx_ra_msg3_t *msg3 = NULL;

  // The ISV app now calls uKE sgx_ra_proc_msg2,
  // The ISV app is responsible for freeing the returned p_msg3!
#ifdef DEBUG
  printf("[RemoteAttestation2] context is %u, eid: %u\n", (uint32_t) context, (uint32_t) eid);
#endif
  ret = sgx_ra_proc_msg2(context,
                         eid,
                         sgx_ra_proc_msg2_trusted,
                         sgx_ra_get_msg3_trusted,
                         p_msg2_body,
                         sizeof(sgx_ra_msg2_t),
                         &msg3,
                         &msg3_size);

  if (!msg3) {
    fprintf(stdout, "\nError, call sgx_ra_proc_msg2 fail. msg3 = 0x%p [%s].\n", msg3, __FUNCTION__);
    print_error_message((sgx_status_t) ret);
    jbyteArray array_ret = env->NewByteArray(0);
    return array_ret;
  }

  if(SGX_SUCCESS != (sgx_status_t)ret) {
    fprintf(stdout, "\nError, call sgx_ra_proc_msg2 fail. "
            "ret = 0x%08x [%s].\n", ret, __FUNCTION__);
    print_error_message((sgx_status_t) ret);
    jbyteArray array_ret = env->NewByteArray(0);
    return array_ret;
  } else {
#ifdef DEBUG
    fprintf(stdout, "\nCall sgx_ra_proc_msg2 success.\n");
#endif
  }

  jbyteArray array_ret = env->NewByteArray(msg3_size);
  env->SetByteArrayRegion(array_ret, 0, msg3_size, (jbyte *) msg3);

  free(msg3);
  return array_ret;
}