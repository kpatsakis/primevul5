Status CheckOpDeprecation(const OpDef& op_def, int graph_def_version) {
  if (op_def.has_deprecation()) {
    const OpDeprecation& dep = op_def.deprecation();
    if (graph_def_version >= dep.version()) {
      return errors::Unimplemented(
          "Op ", op_def.name(), " is not available in GraphDef version ",
          graph_def_version, ". It has been removed in version ", dep.version(),
          ". ", dep.explanation(), ".");
    } else {
      // Warn only once for each op name, and do it in a threadsafe manner.
      static mutex mu(LINKER_INITIALIZED);
      static std::unordered_set<string> warned;
      bool warn;
      {
        mutex_lock lock(mu);
        warn = warned.insert(op_def.name()).second;
      }
      if (warn) {
        LOG(WARNING) << "Op " << op_def.name() << " is deprecated."
                     << " It will cease to work in GraphDef version "
                     << dep.version() << ". " << dep.explanation() << ".";
      }
    }
  }
  return Status::OK();
}