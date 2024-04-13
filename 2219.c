bool IsAllowListedOpTypeForEvaluateNode(const string& op_type) {
  static const gtl::FlatSet<string>* const kOpTpeAllowlist =
      CHECK_NOTNULL((new gtl::FlatSet<string>{
          // Unary arithmetic ops
          "Floor",
          "Round",
          "Sqrt",
          "Square",
          "Sign",
          // Binary arithmetic ops
          "Add",
          "AddV2",
          "Div",
          "FloorDiv",
          "FloorMod",
          "Greater",
          "GreaterEqual",
          "Less",
          "LessEqual",
          "LogicalAnd",
          "LogicalNot",
          "LogicalOr",
          "Maximum",
          "Minimum",
          "Mod",
          "Mul",
          "NotEqual",
          "QuantizedAdd",
          "QuantizedMul",
          "SquareDifference",
          "Sub",
          "TruncateDiv",
          "TruncateMod",
          "RealDiv",
          // N-ary arithmetic ops
          "AddN",
          // Others
          "StridedSlice",
          "OnesLike",
          "ZerosLike",
          "Concat",
          "ConcatV2",
          "Split",
          "Range",
          "Fill",
          "Cast",
          "Prod",
          "Unpack",
          "GatherV2",
          "Pack",
          // Used in batch_gather_nd: tensorflow/python/ops/array_ops.py
          "ExpandDims",
      }));
  return kOpTpeAllowlist->find(op_type) != kOpTpeAllowlist->end();
}