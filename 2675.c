  explicit FractionalAvgPoolGradOp(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("overlapping", &overlapping_));
  }