void TfLiteTensorFree(TfLiteTensor* t) {
  TfLiteTensorDataFree(t);
  if (t->dims) TfLiteIntArrayFree(t->dims);
  t->dims = NULL;

  if (t->dims_signature) {
    TfLiteIntArrayFree((TfLiteIntArray *) t->dims_signature);
  }
  t->dims_signature = NULL;

  TfLiteQuantizationFree(&t->quantization);
  TfLiteSparsityFree(t->sparsity);
  t->sparsity = NULL;
}