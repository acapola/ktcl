// -*- C++ -*-

 //  NOTE: One leading blank has been added to each line of this file
 //        to cope with the demands of the Wiki.  This blank should
 //        be deleted before trying to compile.

 /*
 ## File: TclChannelStreambuf.cpp
 ##
 ## Synopsis:
 ##     Class to support redirection of a C++ stream to a Tcl channel
 ##     (particularly, routing cin/cout/cerr to the console).
 */

 #include "tclchannelstreambuf.h"
#include <algorithm>
#include <cstring>
 //#include <memory.h>

 /*
 ## Function: TclChannelStreambuf::TclChannelStreambuf
 ##
 ## Synopsis:
 ##     Associate a Tcl channel with a streambuf.
 */
 using std::size_t;

TclChannelStreambuf::TclChannelStreambuf (Tcl_Channel fptr, size_t buff_sz, size_t put_back) :
	fptr_ (fptr),
    put_back_(std::max(put_back, size_t(1))),
    buffer_(std::max(buff_sz, put_back_) + put_back_),
    buffer_out(buff_sz + 1)
{
    char *end = &buffer_.front() + buffer_.size();
    setg(end, end, end);
	
	char *base = &buffer_out.front();
    setp(base, base + buffer_out.size() - 1); // -1 to make overflow() easier
}

 /*
 ## Function: TclChannelStreambuf::~TclChannelStreambuf
 ##
 ## Synopsis:
 ##     Destroy a streambuf associated with a Tcl channel.
 */

 /*TclChannelStreambuf::~TclChannelStreambuf () {
 }*/

 /*
 ## Function: TclChannelStreambuf::underflow
 ##
 ## Synopsis:
 ##     Get a fresh input buffer from a Tcl channel for a streambuf
 */
std::streambuf::int_type TclChannelStreambuf::underflow()
{
    if (gptr() < egptr()) // buffer not exhausted
        return traits_type::to_int_type(*gptr());

    char *base = &buffer_.front();
    char *start = base;

    if (eback() == base) // true when this isn't the first fill
    {
        // Make arrangements for putback characters
        std::memmove(base, egptr() - put_back_, put_back_);
        start += put_back_;
    }

    // start is now the start of the buffer, proper.
    // Read from fptr_ in to the provided buffer
	//std::size_t fread( void* buffer, std::size_t size, std::size_t count, std::FILE* stream );
    //size_t n = std::fread(start, 1, buffer_.size() - (start - base), fptr_);
    //int Tcl_ReadChars(channel, readObjPtr, charsToRead, appendFlag)
	//int charsToRead = buffer_.size() - (start - base);
	//int statuc = Tcl_ReadChars(fptr_, start,0);
	//int Tcl_ReadRaw(channel, readBuf, bytesToRead)
	int bytesToRead = buffer_.size() - (start - base);
	size_t n = Tcl_ReadRaw(fptr_, start, bytesToRead);
	if (n == 0)
        return traits_type::eof();

    // Set buffer pointers
    setg(base, start, start + n);

    return traits_type::to_int_type(*gptr());
}
 /*int TclChannelStreambuf::underflow () {
     // Nothing to do if the buffer hasn't underflowed.
     if (egptr() > gptr()) {
        return *gptr ();
     }
     // Flush any pending output
     if (pptr () > pbase ()) {
        if (overflow (EOF) == EOF) {
            return EOF;
        }
     }
     // Get a fresh line of input if needed
     if (iOffset_ >= Tcl_DStringLength (&iBuffer_)) {
        if (Tcl_Gets (channel_, &iBuffer_)) {
            return EOF;
        }
        Tcl_DStringAppend (&iBuffer_, "\n", 1);
     }
     // Determine how much input to transfer.  Don't fill the reserve
     // area more than half full
     size_t xferlen = Tcl_DStringLength (&iBuffer_);
     if ((long) xferlen > (ebuf () - base ()) / 2) {
        xferlen = (ebuf () - base ()) / 2;
     }
     // Copy string into the buffer, and advance pointers
     memcpy ((void *) base (), (void *) Tcl_DStringValue (&iBuffer_), xferlen);
     iOffset_ += xferlen;
     setg (base (), base (), base () + xferlen);
     // Free the input string if we're finished with it
     if (iOffset_ >= Tcl_DStringLength (&iBuffer_)) {
        Tcl_DStringFree (&iBuffer_);
        iOffset_ = 0;
     }
     // Return the first character read.
     return *gptr ();
 }*/

//outputstream stuff
 /*
 ## Function: TclChannelStreambuf::overflow
 ##
 ## Synopsis:
 ##     Handle a full output buffer on a streambuf writing to a Tcl channel.
 */
//virtual streambuf::int_type overflow(streambuf::int_type value)
TclChannelStreambuf::int_type TclChannelStreambuf::overflow(int_type ch) {
    if (ch != traits_type::eof()) {
        //assert(std::less_equal<char *>()(pptr(), epptr()));
        *pptr() = ch;
        pbump(1);
		if (0==sync())
			return ch;//success
    }

    return traits_type::eof();
}
/* int TclChannelStreambuf::overflow (int c) {
     int status;
     
     // If there's no output buffer, allocate one.  Place it after the
     // end of the input buffer if there's input.
     if (!pbase ()) {
        if (egptr () > gptr ()) {
            setp (egptr (), ebuf ());
        } else {
            setp (base (), ebuf ());
        }
     }

     // If there's stuff in the output buffer, write it to the channel.
     if (pptr () > pbase ()) {
        status = Tcl_Write (channel_, pbase (), pptr () - pbase ());
        if (status < (pptr () - pbase ())) {
            return EOF;
        }
        setp (pbase (), ebuf ());
     }

     // Save the next character in the output buffer.  If there is none,
     // flush the channel
     if (c != EOF) {
        *(pptr()) = c;
        pbump (1);
        return c;
     } else {
        setp (0, 0);
        status = Tcl_Flush (channel_);
        if (status != TCL_OK) return EOF;
        return 0;
     }
 }*/


 /*
 ## Function: TclChannelStreambuf::sync
 ##
 ## Synopsis:
 ##     Synchronize input and output on a Tcl channe associated with
 ##     a streambuf
 */


 /*int TclChannelStreambuf::sync () {
     // Flush output
     if (overflow (EOF) == EOF) {
        return EOF;
     }
     // Discard input
     setg (0, 0, 0);
     return 0;
 }*/
int TclChannelStreambuf::sync() {
	int status = Tcl_Write (fptr_, pbase (), pptr () - pbase ());
	if (status < (pptr () - pbase ())) {
		return traits_type::eof();
	}
	status = Tcl_Flush (fptr_);
	if (status != TCL_OK) return traits_type::eof();
	std::ptrdiff_t n = pptr() - pbase();
    pbump(-n);
	return 0;
}
 /*
 ## Function: TclConsoleStreambufSetup
 ##
 ## Synopsis:
 ##     Bind the standard streams to the Tcl console
 */

 /*extern "C" void
 TclConsoleStreambufSetup () {
     cin = new TclChannelStreambuf (Tcl_GetStdChannel (TCL_STDIN));
     cout = new TclChannelStreambuf (Tcl_GetStdChannel (TCL_STDOUT));
     cerr = new TclChannelStreambuf (Tcl_GetStdChannel (TCL_STDERR));
     cerr << "C++ standard input and output now on the Tcl console" << endl;
 }*/
 
 