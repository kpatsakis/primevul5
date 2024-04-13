void RGWGetCrossDomainPolicy_ObjStore_SWIFT::send_response()
{
  set_req_state_err(s, op_ret);
  dump_errno(s);
  end_header(s, this, "application/xml");

  std::stringstream ss;

  ss << R"(<?xml version="1.0"?>)" << "\n"
     << R"(<!DOCTYPE cross-domain-policy SYSTEM )"
     << R"("http://www.adobe.com/xml/dtds/cross-domain-policy.dtd" >)" << "\n"
     << R"(<cross-domain-policy>)" << "\n"
     << g_conf()->rgw_cross_domain_policy << "\n"
     << R"(</cross-domain-policy>)";

  dump_body(s, ss.str());
}