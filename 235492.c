Java_edu_berkeley_cs_rise_opaque_execution_SGXEnclave_PartitionForSort(
  JNIEnv *env, jobject obj, jlong eid, jbyteArray sort_order, jint num_partitions,
  jbyteArray input_rows, jbyteArray boundary_rows) {
  (void)obj;

  jboolean if_copy;

  size_t sort_order_length = static_cast<size_t>(env->GetArrayLength(sort_order));
  uint8_t *sort_order_ptr = reinterpret_cast<uint8_t *>(
    env->GetByteArrayElements(sort_order, &if_copy));

  size_t input_rows_length = static_cast<size_t>(env->GetArrayLength(input_rows));
  uint8_t *input_rows_ptr = reinterpret_cast<uint8_t *>(
    env->GetByteArrayElements(input_rows, &if_copy));

  size_t boundary_rows_length = static_cast<size_t>(env->GetArrayLength(boundary_rows));
  uint8_t *boundary_rows_ptr = reinterpret_cast<uint8_t *>(
    env->GetByteArrayElements(boundary_rows, &if_copy));

  uint8_t **output_partitions = new uint8_t *[num_partitions];
  size_t *output_partition_lengths = new size_t[num_partitions];

  sgx_check("Partition For Sort",
            ecall_partition_for_sort(
              eid,
              sort_order_ptr, sort_order_length,
              num_partitions,
              input_rows_ptr, input_rows_length,
              boundary_rows_ptr, boundary_rows_length,
              output_partitions, output_partition_lengths));

  env->ReleaseByteArrayElements(sort_order, reinterpret_cast<jbyte *>(sort_order_ptr), 0);
  env->ReleaseByteArrayElements(input_rows, reinterpret_cast<jbyte *>(input_rows_ptr), 0);
  env->ReleaseByteArrayElements(boundary_rows, reinterpret_cast<jbyte *>(boundary_rows_ptr), 0);

  jobjectArray result = env->NewObjectArray(num_partitions,  env->FindClass("[B"), nullptr);
  for (jint i = 0; i < num_partitions; i++) {
    jbyteArray partition = env->NewByteArray(output_partition_lengths[i]);
    env->SetByteArrayRegion(partition, 0, output_partition_lengths[i],
                            reinterpret_cast<jbyte *>(output_partitions[i]));
    free(output_partitions[i]);
    env->SetObjectArrayElement(result, i, partition);
  }
  delete[] output_partitions;
  delete[] output_partition_lengths;

  return result;
}