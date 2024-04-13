JNIEXPORT jlong JNICALL Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_StartEnclave(
  JNIEnv *env, jobject obj, jstring library_path) {
  (void)env;
  (void)obj;

  env->GetJavaVM(&jvm);

  sgx_enclave_id_t eid;
  sgx_launch_token_t token = {0};
  int updated = 0;

  const char *library_path_str = env->GetStringUTFChars(library_path, nullptr);
  sgx_check("StartEnclave",
            sgx_create_enclave(
              library_path_str, SGX_DEBUG_FLAG, &token, &updated, &eid, nullptr));
  env->ReleaseStringUTFChars(library_path, library_path_str);

  return eid;
}