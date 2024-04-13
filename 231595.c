LineBufferTaskIIF::~LineBufferTaskIIF ()
{
     //
     // Signal that the line buffer is now free
     //
     
     _lineBuffer->post ();
}