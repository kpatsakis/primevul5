JNIEXPORT jbyteArray JNICALL Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_ExternalSort(
  JNIEnv *env, jobject obj, jlong eid, jbyteArray sort_order, jbyteArray input_rows) {
  (void)obj;

  jboolean if_copy;

  size_t sort_order_length = static_cast<size_t>(env->GetArrayLength(sort_order));
  uint8_t *sort_order_ptr = reinterpret_cast<uint8_t *>(
    env->GetByteArrayElements(sort_order, &if_copy));

  size_t input_rows_length = static_cast<size_t>(env->GetArrayLength(input_rows));
  uint8_t *input_rows_ptr = reinterpret_cast<uint8_t *>(
    env->GetByteArrayElements(input_rows, &if_copy));

  uint8_t *output_rows;
  size_t output_rows_length;

  sgx_check("External non-oblivious sort",
            ecall_external_sort(eid,
                                sort_order_ptr, sort_order_length,
                                input_rows_ptr, input_rows_length,
                                &output_rows, &output_rows_length));

  jbyteArray ret = env->NewByteArray(output_rows_length);
  env->SetByteArrayRegion(ret, 0, output_rows_length, reinterpret_cast<jbyte *>(output_rows));
  free(output_rows);

  env->ReleaseByteArrayElements(sort_order, reinterpret_cast<jbyte *>(sort_order_ptr), 0);
  env->ReleaseByteArrayElements(input_rows, reinterpret_cast<jbyte *>(input_rows_ptr), 0);

  return ret;
}