HttpStateData::abortAll(const char *reason)
{
    debugs(11,5, HERE << "aborting transaction for " << reason <<
           "; " << serverConnection << ", this " << this);
    mustStop(reason);
}