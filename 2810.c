void RemoveDescriptionsFromOpList(OpList* op_list) {
  for (int i = 0; i < op_list->op_size(); ++i) {
    OpDef* op_def = op_list->mutable_op(i);
    RemoveDescriptionsFromOpDef(op_def);
  }
}