  Status Merge(DimensionHandle d1, DimensionHandle d2) {
    if (!d1.IsSet() || !d2.IsSet()) {
      return Status::OK();
    }
    return dims_.Merge(d1, d2);
  }