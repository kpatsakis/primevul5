void TfLiteSparsityFree(TfLiteSparsity* sparsity) {
  if (sparsity == NULL) {
    return;
  }

  if (sparsity->traversal_order) {
    TfLiteIntArrayFree(sparsity->traversal_order);
    sparsity->traversal_order = NULL;
  }

  if (sparsity->block_map) {
    TfLiteIntArrayFree(sparsity->block_map);
    sparsity->block_map = NULL;
  }

  if (sparsity->dim_metadata) {
    int i = 0;
    for (; i < sparsity->dim_metadata_size; i++) {
      TfLiteDimensionMetadata metadata = sparsity->dim_metadata[i];
      if (metadata.format == kTfLiteDimSparseCSR) {
        TfLiteIntArrayFree(metadata.array_segments);
        metadata.array_segments = NULL;
        TfLiteIntArrayFree(metadata.array_indices);
        metadata.array_indices = NULL;
      }
    }
    free(sparsity->dim_metadata);
    sparsity->dim_metadata = NULL;
  }

  free(sparsity);
}