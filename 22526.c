ciMethod* ciEnv::get_method_by_index_impl(const constantPoolHandle& cpool,
                                          int index, Bytecodes::Code bc,
                                          ciInstanceKlass* accessor) {
  assert(cpool.not_null(), "need constant pool");
  assert(accessor != NULL, "need origin of access");
  if (bc == Bytecodes::_invokedynamic) {
    ConstantPoolCacheEntry* cpce = cpool->invokedynamic_cp_cache_entry_at(index);
    bool is_resolved = !cpce->is_f1_null();
    // FIXME: code generation could allow for null (unlinked) call site
    // The call site could be made patchable as follows:
    // Load the appendix argument from the constant pool.
    // Test the appendix argument and jump to a known deopt routine if it is null.
    // Jump through a patchable call site, which is initially a deopt routine.
    // Patch the call site to the nmethod entry point of the static compiled lambda form.
    // As with other two-component call sites, both values must be independently verified.

    if (is_resolved) {
      // Get the invoker Method* from the constant pool.
      // (The appendix argument, if any, will be noted in the method's signature.)
      Method* adapter = cpce->f1_as_method();
      return get_method(adapter);
    }

    // Fake a method that is equivalent to a declared method.
    ciInstanceKlass* holder    = get_instance_klass(vmClasses::MethodHandle_klass());
    ciSymbol*        name      = ciSymbols::invokeBasic_name();
    ciSymbol*        signature = get_symbol(cpool->signature_ref_at(index));
    return get_unloaded_method(holder, name, signature, accessor);
  } else {
    const int holder_index = cpool->klass_ref_index_at(index);
    bool holder_is_accessible;
    ciKlass* holder = get_klass_by_index_impl(cpool, holder_index, holder_is_accessible, accessor);

    // Get the method's name and signature.
    Symbol* name_sym = cpool->name_ref_at(index);
    Symbol* sig_sym  = cpool->signature_ref_at(index);

    if (cpool->has_preresolution()
        || ((holder == ciEnv::MethodHandle_klass() || holder == ciEnv::VarHandle_klass()) &&
            MethodHandles::is_signature_polymorphic_name(holder->get_Klass(), name_sym))) {
      // Short-circuit lookups for JSR 292-related call sites.
      // That is, do not rely only on name-based lookups, because they may fail
      // if the names are not resolvable in the boot class loader (7056328).
      switch (bc) {
      case Bytecodes::_invokevirtual:
      case Bytecodes::_invokeinterface:
      case Bytecodes::_invokespecial:
      case Bytecodes::_invokestatic:
        {
          Method* m = ConstantPool::method_at_if_loaded(cpool, index);
          if (m != NULL) {
            return get_method(m);
          }
        }
        break;
      default:
        break;
      }
    }

    if (holder_is_accessible) {  // Our declared holder is loaded.
      constantTag tag = cpool->tag_ref_at(index);
      assert(accessor->get_instanceKlass() == cpool->pool_holder(), "not the pool holder?");
      Method* m = lookup_method(accessor, holder, name_sym, sig_sym, bc, tag);
      if (m != NULL &&
          (bc == Bytecodes::_invokestatic
           ?  m->method_holder()->is_not_initialized()
           : !m->method_holder()->is_loaded())) {
        m = NULL;
      }
#ifdef ASSERT
      if (m != NULL && ReplayCompiles && !ciReplay::is_loaded(m)) {
        m = NULL;
      }
#endif
      if (m != NULL) {
        // We found the method.
        return get_method(m);
      }
    }

    // Either the declared holder was not loaded, or the method could
    // not be found.  Create a dummy ciMethod to represent the failed
    // lookup.
    ciSymbol* name      = get_symbol(name_sym);
    ciSymbol* signature = get_symbol(sig_sym);
    return get_unloaded_method(holder, name, signature, accessor);
  }
}