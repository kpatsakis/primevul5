httpStart(FwdState *fwd)
{
    debugs(11, 3, fwd->request->method << ' ' << fwd->entry->url());
    AsyncJob::Start(new HttpStateData(fwd));
}