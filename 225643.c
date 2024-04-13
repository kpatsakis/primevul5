void RGWAsioFrontend::unpause_with_new_config(
  rgw::sal::RGWRadosStore* const store,
  rgw_auth_registry_ptr_t auth_registry
) {
  impl->unpause(store, std::move(auth_registry));
}