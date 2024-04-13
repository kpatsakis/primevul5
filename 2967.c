void TfLiteTensorDataFree(TfLiteTensor* t) {
  if (t->allocation_type == kTfLiteDynamic ||
      t->allocation_type == kTfLitePersistentRo) {
    free(t->data.raw);
  }
  t->data.raw = NULL;
}