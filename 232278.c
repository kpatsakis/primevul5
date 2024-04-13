RefreshTimeOutProcess (
  IN  EFI_EVENT    Event,
  IN  VOID         *Context
  )
{
  WARNING_IF_CONTEXT     *EventInfo;
  CHAR16                 TimeOutString[MAX_TIME_OUT_LEN];

  EventInfo   = (WARNING_IF_CONTEXT *) Context;

  if (*(EventInfo->TimeOut) == 0) {
    gBS->CloseEvent (Event);

    gBS->SignalEvent (EventInfo->SyncEvent);
    return;
  }

  UnicodeSPrint(TimeOutString, MAX_TIME_OUT_LEN, L"%d", *(EventInfo->TimeOut));

  CreateDialog (NULL, gEmptyString, EventInfo->ErrorInfo, gPressEnter, gEmptyString, TimeOutString, NULL);

  *(EventInfo->TimeOut) -= 1;
}