
 // -*- C++ -*-

 //  NOTE: One leading blank has been added to each line of this file
 //        to cope with the demands of the Wiki.  This blank should
 //        be deleted before trying to compile.

 /*
 ## File: TclChannelStreambuf.h
 ##
 ## Synopsis:
 ##      Class to support redirection of a C++ stream to a Tcl channel
 ##      (particularly, routing cin/cout/cerr to the console).
 #@CDef
 */

 #ifndef __TclChannelStreambuf_h
 #define __TclChannelStreambuf_h 1

 extern "C" {
 #include <tcl.h>
 }

#include <iostream>
#include <streambuf>
#include <vector>
//#include <cstdlib>
//#include <cstdio>

 class TclChannelStreambuf : public std::streambuf {
 public:
        explicit TclChannelStreambuf(Tcl_Channel fptr, std::size_t buff_sz = 256, std::size_t put_back = 8);

    private:
        // overrides base class underflow()
        int_type underflow();
		//for outputstream support:
		int_type overflow(int_type ch);
        int sync();

        // copy ctor and assignment not implemented;
        // copying not allowed
        TclChannelStreambuf(const TclChannelStreambuf &);
        TclChannelStreambuf &operator= (const TclChannelStreambuf &);

    private:
        Tcl_Channel fptr_;
        const std::size_t put_back_;
        std::vector<char> buffer_;
		
        //std::ostream &sink_;
        std::vector<char> buffer_out;
 };

class tclchannel_istream: public std::istream {
public:
    tclchannel_istream(Tcl_Channel fptr) :
        std::istream(new TclChannelStreambuf(fptr)) {}

    virtual ~tclchannel_istream() {
        delete rdbuf();
    }
};


/*** tclchannel_ostream class ********************************************/

class tclchannel_ostream: public std::ostream {
public:
    tclchannel_ostream(Tcl_Channel fptr) :
        std::ostream(new TclChannelStreambuf(fptr)) {}

    virtual ~tclchannel_ostream() {
        delete rdbuf();
    }
};


 extern "C" void TclConsoleStreambufSetup _ANSI_ARGS_(( void ));

 #endif /* __TclChannelStreambuf_h */