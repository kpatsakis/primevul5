Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_NonObliviousSortMergeJoin(
  JNIEnv *env, jobject obj, jlong eid, jbyteArray join_expr, jbyteArray input_rows,
  jbyteArray join_row) {
  (void)obj;

  jboolean if_copy;

  uint32_t join_expr_length = (uint32_t) env->GetArrayLength(join_expr);
  uint8_t *join_expr_ptr = (uint8_t *) env->GetByteArrayElements(join_expr, &if_copy);

  uint32_t input_rows_length = (uint32_t) env->GetArrayLength(input_rows);
  uint8_t *input_rows_ptr = (uint8_t *) env->GetByteArrayElements(input_rows, &if_copy);

  uint32_t join_row_length = (uint32_t) env->GetArrayLength(join_row);
  uint8_t *join_row_ptr = (uint8_t *) env->GetByteArrayElements(join_row, &if_copy);

  uint8_t *output_rows;
  size_t output_rows_length;

  sgx_check("Non-oblivious SortMergeJoin",
            ecall_non_oblivious_sort_merge_join(
              eid,
              join_expr_ptr, join_expr_length,
              input_rows_ptr, input_rows_length,
              join_row_ptr, join_row_length,
              &output_rows, &output_rows_length));
  
  jbyteArray ret = env->NewByteArray(output_rows_length);
  env->SetByteArrayRegion(ret, 0, output_rows_length, (jbyte *) output_rows);
  free(output_rows);

  env->ReleaseByteArrayElements(join_expr, (jbyte *) join_expr_ptr, 0);
  env->ReleaseByteArrayElements(input_rows, (jbyte *) input_rows_ptr, 0);
  env->ReleaseByteArrayElements(join_row, (jbyte *) join_row_ptr, 0);

  return ret;
}