Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_NonObliviousAggregateStep2(
  JNIEnv *env, jobject obj, jlong eid, jbyteArray agg_op, jbyteArray input_rows,
  jbyteArray next_partition_first_row, jbyteArray prev_partition_last_group,
  jbyteArray prev_partition_last_row) {
  (void)obj;

  jboolean if_copy;

  uint32_t agg_op_length = (uint32_t) env->GetArrayLength(agg_op);
  uint8_t *agg_op_ptr = (uint8_t *) env->GetByteArrayElements(agg_op, &if_copy);

  uint32_t input_rows_length = (uint32_t) env->GetArrayLength(input_rows);
  uint8_t *input_rows_ptr = (uint8_t *) env->GetByteArrayElements(input_rows, &if_copy);

  uint32_t next_partition_first_row_length =
    (uint32_t) env->GetArrayLength(next_partition_first_row);
  uint8_t *next_partition_first_row_ptr =
    (uint8_t *) env->GetByteArrayElements(next_partition_first_row, &if_copy);

  uint32_t prev_partition_last_group_length =
    (uint32_t) env->GetArrayLength(prev_partition_last_group);
  uint8_t *prev_partition_last_group_ptr =
    (uint8_t *) env->GetByteArrayElements(prev_partition_last_group, &if_copy);

  uint32_t prev_partition_last_row_length =
    (uint32_t) env->GetArrayLength(prev_partition_last_row);
  uint8_t *prev_partition_last_row_ptr =
    (uint8_t *) env->GetByteArrayElements(prev_partition_last_row, &if_copy);

  uint8_t *output_rows;
  size_t output_rows_length;

  sgx_check("Non-Oblivious Aggregate Step 2",
            ecall_non_oblivious_aggregate_step2(
              eid,
              agg_op_ptr, agg_op_length,
              input_rows_ptr, input_rows_length,
              next_partition_first_row_ptr, next_partition_first_row_length,
              prev_partition_last_group_ptr, prev_partition_last_group_length,
              prev_partition_last_row_ptr, prev_partition_last_row_length,
              &output_rows, &output_rows_length));

  jbyteArray ret = env->NewByteArray(output_rows_length);
  env->SetByteArrayRegion(ret, 0, output_rows_length, (jbyte *) output_rows);
  free(output_rows);

  env->ReleaseByteArrayElements(agg_op, (jbyte *) agg_op_ptr, 0);
  env->ReleaseByteArrayElements(input_rows, (jbyte *) input_rows_ptr, 0);
  env->ReleaseByteArrayElements(
    next_partition_first_row, (jbyte *) next_partition_first_row_ptr, 0);
  env->ReleaseByteArrayElements(
    prev_partition_last_group, (jbyte *) prev_partition_last_group_ptr, 0);
  env->ReleaseByteArrayElements(
    prev_partition_last_row, (jbyte *) prev_partition_last_row_ptr, 0);

  return ret;
}