bool IsDequeue(const NodeDef& n) {
  return (n.op().find("Dequeue") != string::npos &&
          n.op().find("DequeueMany") == string::npos);
}