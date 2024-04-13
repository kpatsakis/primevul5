bool ChildProcessSecurityPolicy::CanRequestURL(
    int renderer_id, const GURL& url) {
  if (!url.is_valid())
    return false;  // Can't request invalid URLs.

  if (IsWebSafeScheme(url.scheme()))
    return true;  // The scheme has been white-listed for every renderer.

  if (IsPseudoScheme(url.scheme())) {

     if (url.SchemeIs(chrome::kViewSourceScheme) ||
         url.SchemeIs(chrome::kPrintScheme)) {
      return CanRequestURL(renderer_id, GURL(url.path()));
     }
 
     if (LowerCaseEqualsASCII(url.spec(), chrome::kAboutBlankURL))
      return true;  // Every renderer can request <about:blank>.

    return false;
  }

  if (!URLRequest::IsHandledURL(url))
    return true;  // This URL request is destined for ShellExecute.

  {
    AutoLock lock(lock_);

    SecurityStateMap::iterator state = security_state_.find(renderer_id);
    if (state == security_state_.end())
      return false;

    return state->second->CanRequestURL(url);
  }
}
