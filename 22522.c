void ciEnv::cache_dtrace_flags() {
  // Need lock?
  _dtrace_extended_probes = ExtendedDTraceProbes;
  if (_dtrace_extended_probes) {
    _dtrace_method_probes   = true;
    _dtrace_alloc_probes    = true;
  } else {
    _dtrace_method_probes   = DTraceMethodProbes;
    _dtrace_alloc_probes    = DTraceAllocProbes;
  }
}