TfLiteDelegate TfLiteDelegateCreate(void) {
  TfLiteDelegate d = {
      .data_ = NULL,
      .Prepare = NULL,
      .CopyFromBufferHandle = NULL,
      .CopyToBufferHandle = NULL,
      .FreeBufferHandle = NULL,
      .flags = kTfLiteDelegateFlagsNone,
  };
  return d;
}