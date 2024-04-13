static int ctrl_error(void)
{
    if (errno == ETIMEDOUT)
        return USB_RET_NAK;
    else 
        return USB_RET_STALL;
}