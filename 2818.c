  explicit StringNGramsOp(tensorflow::OpKernelConstruction* context)
      : tensorflow::OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("separator", &separator_));
    OP_REQUIRES_OK(context, context->GetAttr("ngram_widths", &ngram_widths_));
    OP_REQUIRES_OK(context, context->GetAttr("left_pad", &left_pad_));
    OP_REQUIRES_OK(context, context->GetAttr("right_pad", &right_pad_));
    OP_REQUIRES_OK(context, context->GetAttr("pad_width", &pad_width_));
    OP_REQUIRES_OK(context, context->GetAttr("preserve_short_sequences",
                                             &preserve_short_));
  }