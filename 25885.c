int my_aio_error(const struct aiocb *aiocbp)
{
   return(aiocbp->aio_lio_opcode);
}