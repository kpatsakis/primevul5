bool IsEnqueue(const NodeDef& n) {
  return (n.op().find("Enqueue") != string::npos &&
          n.op().find("EnqueueMany") == string::npos);
}