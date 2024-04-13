Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_NonObliviousAggregateStep1(
  JNIEnv *env, jobject obj, jlong eid, jbyteArray agg_op, jbyteArray input_rows) {
  (void)obj;

  jboolean if_copy;

  uint32_t agg_op_length = (uint32_t) env->GetArrayLength(agg_op);
  uint8_t *agg_op_ptr = (uint8_t *) env->GetByteArrayElements(agg_op, &if_copy);

  uint32_t input_rows_length = (uint32_t) env->GetArrayLength(input_rows);
  uint8_t *input_rows_ptr = (uint8_t *) env->GetByteArrayElements(input_rows, &if_copy);

  uint8_t *first_row;
  size_t first_row_length;

  uint8_t *last_group;
  size_t last_group_length;

  uint8_t *last_row;
  size_t last_row_length;

  sgx_check("Non-Oblivious Aggregate Step 1",
            ecall_non_oblivious_aggregate_step1(
              eid,
              agg_op_ptr, agg_op_length,
              input_rows_ptr, input_rows_length,
              &first_row, &first_row_length,
              &last_group, &last_group_length,
              &last_row, &last_row_length));

  jbyteArray first_row_array = env->NewByteArray(first_row_length);
  env->SetByteArrayRegion(first_row_array, 0, first_row_length, (jbyte *) first_row);
  free(first_row);

  jbyteArray last_group_array = env->NewByteArray(last_group_length);
  env->SetByteArrayRegion(last_group_array, 0, last_group_length, (jbyte *) last_group);
  free(last_group);

  jbyteArray last_row_array = env->NewByteArray(last_row_length);
  env->SetByteArrayRegion(last_row_array, 0, last_row_length, (jbyte *) last_row);
  free(last_row);

  env->ReleaseByteArrayElements(agg_op, (jbyte *) agg_op_ptr, 0);
  env->ReleaseByteArrayElements(input_rows, (jbyte *) input_rows_ptr, 0);

  jclass tuple3_class = env->FindClass("scala/Tuple3");
  jobject ret = env->NewObject(
    tuple3_class,
    env->GetMethodID(tuple3_class, "<init>",
                     "(Ljava/lang/Object;Ljava/lang/Object;Ljava/lang/Object;)V"),
    first_row_array, last_group_array, last_row_array);

  return ret;
}