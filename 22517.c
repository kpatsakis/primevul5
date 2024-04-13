Method* ciEnv::lookup_method(ciInstanceKlass* accessor,
                             ciKlass*         holder,
                             Symbol*          name,
                             Symbol*          sig,
                             Bytecodes::Code  bc,
                             constantTag      tag) {
  InstanceKlass* accessor_klass = accessor->get_instanceKlass();
  Klass* holder_klass = holder->get_Klass();

  // Accessibility checks are performed in ciEnv::get_method_by_index_impl.
  assert(check_klass_accessibility(accessor, holder_klass), "holder not accessible");

  LinkInfo link_info(holder_klass, name, sig, accessor_klass,
                     LinkInfo::AccessCheck::required,
                     LinkInfo::LoaderConstraintCheck::required,
                     tag);
  switch (bc) {
    case Bytecodes::_invokestatic:
      return LinkResolver::resolve_static_call_or_null(link_info);
    case Bytecodes::_invokespecial:
      return LinkResolver::resolve_special_call_or_null(link_info);
    case Bytecodes::_invokeinterface:
      return LinkResolver::linktime_resolve_interface_method_or_null(link_info);
    case Bytecodes::_invokevirtual:
      return LinkResolver::linktime_resolve_virtual_method_or_null(link_info);
    default:
      fatal("Unhandled bytecode: %s", Bytecodes::name(bc));
      return NULL; // silence compiler warnings
  }
}