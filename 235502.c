JNIEXPORT jbyteArray JNICALL Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_Filter(
  JNIEnv *env, jobject obj, jlong eid, jbyteArray condition, jbyteArray input_rows) {
  (void)obj;

  jboolean if_copy;

  size_t condition_length = (size_t) env->GetArrayLength(condition);
  uint8_t *condition_ptr = (uint8_t *) env->GetByteArrayElements(condition, &if_copy);

  uint32_t input_rows_length = (uint32_t) env->GetArrayLength(input_rows);
  uint8_t *input_rows_ptr = (uint8_t *) env->GetByteArrayElements(input_rows, &if_copy);

  uint8_t *output_rows;
  size_t output_rows_length;

  sgx_check("Filter",
            ecall_filter(
              eid,
              condition_ptr, condition_length,
              input_rows_ptr, input_rows_length,
              &output_rows, &output_rows_length));

  env->ReleaseByteArrayElements(condition, (jbyte *) condition_ptr, 0);
  env->ReleaseByteArrayElements(input_rows, (jbyte *) input_rows_ptr, 0);

  jbyteArray ret = env->NewByteArray(output_rows_length);
  env->SetByteArrayRegion(ret, 0, output_rows_length, (jbyte *) output_rows);
  free(output_rows);

  return ret;
}