bool GraphProperties::HasInputProperties(const string& node_name) const {
  return input_properties_.find(node_name) != input_properties_.end();
}