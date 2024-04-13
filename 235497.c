JNIEXPORT jbyteArray JNICALL Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_Project(
  JNIEnv *env, jobject obj, jlong eid, jbyteArray project_list, jbyteArray input_rows) {
  (void)obj;

  jboolean if_copy;

  uint32_t project_list_length = (uint32_t) env->GetArrayLength(project_list);
  uint8_t *project_list_ptr = (uint8_t *) env->GetByteArrayElements(project_list, &if_copy);

  uint32_t input_rows_length = (uint32_t) env->GetArrayLength(input_rows);
  uint8_t *input_rows_ptr = (uint8_t *) env->GetByteArrayElements(input_rows, &if_copy);

  uint8_t *output_rows;
  size_t output_rows_length;

  sgx_check("Project",
            ecall_project(
              eid,
              project_list_ptr, project_list_length,
              input_rows_ptr, input_rows_length,
              &output_rows, &output_rows_length));

  env->ReleaseByteArrayElements(project_list, (jbyte *) project_list_ptr, 0);
  env->ReleaseByteArrayElements(input_rows, (jbyte *) input_rows_ptr, 0);

  jbyteArray ret = env->NewByteArray(output_rows_length);
  env->SetByteArrayRegion(ret, 0, output_rows_length, (jbyte *) output_rows);
  free(output_rows);

  return ret;
}