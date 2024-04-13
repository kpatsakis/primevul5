ciKlass* ciEnv::get_klass_by_name_impl(ciKlass* accessing_klass,
                                       const constantPoolHandle& cpool,
                                       ciSymbol* name,
                                       bool require_local) {
  ASSERT_IN_VM;
  Thread* current = Thread::current();

  // Now we need to check the SystemDictionary
  Symbol* sym = name->get_symbol();
  if (Signature::has_envelope(sym)) {
    // This is a name from a signature.  Strip off the trimmings.
    // Call recursive to keep scope of strippedsym.
    TempNewSymbol strippedsym = Signature::strip_envelope(sym);
    ciSymbol* strippedname = get_symbol(strippedsym);
    return get_klass_by_name_impl(accessing_klass, cpool, strippedname, require_local);
  }

  // Check for prior unloaded klass.  The SystemDictionary's answers
  // can vary over time but the compiler needs consistency.
  ciKlass* unloaded_klass = check_get_unloaded_klass(accessing_klass, name);
  if (unloaded_klass != NULL) {
    if (require_local)  return NULL;
    return unloaded_klass;
  }

  Handle loader;
  Handle domain;
  if (accessing_klass != NULL) {
    loader = Handle(current, accessing_klass->loader());
    domain = Handle(current, accessing_klass->protection_domain());
  }

  Klass* found_klass;
  {
    ttyUnlocker ttyul;  // release tty lock to avoid ordering problems
    MutexLocker ml(current, Compile_lock);
    Klass* kls;
    if (!require_local) {
      kls = SystemDictionary::find_constrained_instance_or_array_klass(current, sym, loader);
    } else {
      kls = SystemDictionary::find_instance_or_array_klass(sym, loader, domain);
    }
    found_klass = kls;
  }

  // If we fail to find an array klass, look again for its element type.
  // The element type may be available either locally or via constraints.
  // In either case, if we can find the element type in the system dictionary,
  // we must build an array type around it.  The CI requires array klasses
  // to be loaded if their element klasses are loaded, except when memory
  // is exhausted.
  if (Signature::is_array(sym) &&
      (sym->char_at(1) == JVM_SIGNATURE_ARRAY || sym->char_at(1) == JVM_SIGNATURE_CLASS)) {
    // We have an unloaded array.
    // Build it on the fly if the element class exists.
    SignatureStream ss(sym, false);
    ss.skip_array_prefix(1);
    // Get element ciKlass recursively.
    ciKlass* elem_klass =
      get_klass_by_name_impl(accessing_klass,
                             cpool,
                             get_symbol(ss.as_symbol()),
                             require_local);
    if (elem_klass != NULL && elem_klass->is_loaded()) {
      // Now make an array for it
      return ciObjArrayKlass::make_impl(elem_klass);
    }
  }

  if (found_klass == NULL && !cpool.is_null() && cpool->has_preresolution()) {
    // Look inside the constant pool for pre-resolved class entries.
    for (int i = cpool->length() - 1; i >= 1; i--) {
      if (cpool->tag_at(i).is_klass()) {
        Klass* kls = cpool->resolved_klass_at(i);
        if (kls->name() == sym) {
          found_klass = kls;
          break;
        }
      }
    }
  }

  if (found_klass != NULL) {
    // Found it.  Build a CI handle.
    return get_klass(found_klass);
  }

  if (require_local)  return NULL;

  // Not yet loaded into the VM, or not governed by loader constraints.
  // Make a CI representative for it.
  return get_unloaded_klass(accessing_klass, name);
}