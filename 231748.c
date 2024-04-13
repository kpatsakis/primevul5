static inline int handle_metadata_errors(req_state* const s, const int op_ret)
{
  if (op_ret == -EFBIG) {
    /* Handle the custom error message of exceeding maximum custom attribute
     * (stored as xattr) size. */
    const auto error_message = boost::str(
      boost::format("Metadata value longer than %lld")
        % s->cct->_conf.get_val<Option::size_t>("rgw_max_attr_size"));
    set_req_state_err(s, EINVAL, error_message);
    return -EINVAL;
  } else if (op_ret == -E2BIG) {
    const auto error_message = boost::str(
      boost::format("Too many metadata items; max %lld")
        % s->cct->_conf.get_val<uint64_t>("rgw_max_attrs_num_in_req"));
    set_req_state_err(s, EINVAL, error_message);
    return -EINVAL;
  }

  return op_ret;
}