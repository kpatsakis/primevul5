bool ciEnv::check_klass_accessibility(ciKlass* accessing_klass,
                                      Klass* resolved_klass) {
  if (accessing_klass == NULL || !accessing_klass->is_loaded()) {
    return true;
  }
  if (accessing_klass->is_obj_array_klass()) {
    accessing_klass = accessing_klass->as_obj_array_klass()->base_element_klass();
  }
  if (!accessing_klass->is_instance_klass()) {
    return true;
  }

  if (resolved_klass->is_objArray_klass()) {
    // Find the element klass, if this is an array.
    resolved_klass = ObjArrayKlass::cast(resolved_klass)->bottom_klass();
  }
  if (resolved_klass->is_instance_klass()) {
    return (Reflection::verify_class_access(accessing_klass->get_Klass(),
                                            InstanceKlass::cast(resolved_klass),
                                            true) == Reflection::ACCESS_OK);
  }
  return true;
}