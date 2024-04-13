size_t my_aio_return(struct aiocb *aiocbp)
{
   return(aiocbp->aio_nbytes);
}