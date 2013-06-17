
/* $Header: //xaptk/include/xapex.h#8 $ */
/* xapex.h */

/*
ADOBE SYSTEMS INCORPORATED
Copyright 2001 Adobe Systems Incorporated
All Rights Reserved

NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the 
terms of the Adobe license agreement accompanying it.
*/


#ifndef XAPEX_H
#define XAPEX_H /* as nothing */


/*
Classes used for exceptions.
*/

#include <stdexcept>
#include <string>

#if defined(WITHIN_PS) && (WITHIN_PS == 0)
/*
This is unfortunate, but necessary in order to make both the Acrobat
Viewer and Distiller happy.
*/
#undef WITHIN_PS
#endif

#ifndef WITHIN_PS
#include "XAPConfigure.h"
#endif /* WITHIN_PS */

#ifdef NEED_BAD_ALLOC

namespace std {
class bad_alloc : public exception {
public:
    bad_alloc() throw();
    bad_alloc(const bad_alloc&) throw();
    bad_alloc& operator=(const bad_alloc&) throw();
    virtual ~bad_alloc() throw();
    virtual const char* what() const throw();
};
}

#else
#include <new>
#endif // NEED_BAD_ALLOC


#if macintosh	// Both MrC and Code Warrior define this automatically.
	#pragma options align=native
#endif


/* Text messages for standard exceptions. */
extern const char *const XAP_BAD_ALLOC;
extern const char *const XAP_INVALID_ARGUMENT;

/* Text messages for client faults. */
extern const char *const XAP_FAULT_BAD_FEATURE;
extern const char *const XAP_FAULT_BAD_SCHEMA;
extern const char *const XAP_FAULT_BAD_TYPE;
extern const char *const XAP_FAULT_BAD_PATH;
extern const char *const XAP_FAULT_BAD_ACCESS;
extern const char *const XAP_FAULT_BAD_NUMBER;

/* Text messages for XAP errors. */
extern const char *const XAP_ERR_BAD_XAP;
extern const char *const XAP_ERR_BAD_XML;
extern const char *const XAP_ERR_NO_MATCH;
extern const char *const XAP_ERR_ASSERT;

class xap_client_fault : public std::logic_error {
public:
    virtual ~xap_client_fault() throw() {}
protected:
    xap_client_fault() throw() : std::logic_error("") {}
    explicit xap_client_fault(const char* w) throw() :
      std::logic_error(w) {}
};

class xap_error : public std::runtime_error {
public:
    virtual ~xap_error() throw() {}
    virtual const char* getContext() const throw() {
        return(m_context.c_str());
    }
    virtual const int getLine() const throw() {
        return(m_line);
    }
protected:
    xap_error() throw() : std::runtime_error("") {}
    explicit xap_error(const char *const w) throw() : std::runtime_error(w) {}
    virtual void setContext(const char* c) {
        m_context = c;
    }
    virtual void setLine(const int line) {
        m_line = line;
    }
private:
    std::string m_context;
    int m_line;
};

class xap_bad_feature : public xap_client_fault {
public:
    xap_bad_feature() throw() : xap_client_fault(XAP_FAULT_BAD_FEATURE) {}
};

class xap_bad_schema : public xap_client_fault {
public:
    xap_bad_schema() throw() : xap_client_fault(XAP_FAULT_BAD_SCHEMA) {}
};

class xap_bad_type : public xap_client_fault {
public:
    xap_bad_type() throw() : xap_client_fault(XAP_FAULT_BAD_TYPE) {}
};

class xap_bad_path : public xap_client_fault {
public:
    xap_bad_path() throw() : xap_client_fault(XAP_FAULT_BAD_PATH) {}
};

class xap_bad_access : public xap_client_fault {
public:
    xap_bad_access() throw() : xap_client_fault(XAP_FAULT_BAD_ACCESS) {}
};

class xap_bad_number : public xap_client_fault {
public:
    xap_bad_number() throw() : xap_client_fault(XAP_FAULT_BAD_NUMBER) {}
};

class xap_bad_xap : public xap_error {
public:
    xap_bad_xap() throw() : xap_error(XAP_ERR_BAD_XAP) {}
    explicit xap_bad_xap(const char *const c) : xap_error(XAP_ERR_BAD_XAP) {
        setContext(c);
        setLine(0);
    }
};

class xap_bad_xml : public xap_error {
public:
    xap_bad_xml() throw() : xap_error(XAP_ERR_BAD_XML) {}
    xap_bad_xml(const char *const c, const int l) : xap_error(XAP_ERR_BAD_XML) {
        setContext(c);
        setLine(l);
    }
};

class xap_no_match : public xap_error {
public:
    xap_no_match() throw() : xap_error(XAP_ERR_NO_MATCH) {}
    explicit xap_no_match(const char *const path) : xap_error(XAP_ERR_NO_MATCH) {
        setContext(path);
        setLine(0);
    }
};

class xap_assert_failure : public xap_error {
public:
    xap_assert_failure() throw() : xap_error(XAP_ERR_ASSERT) {}
    explicit xap_assert_failure(const char *const path) : xap_error(XAP_ERR_ASSERT) {
        setContext(path);
        setLine(0);
    }
};


#if macintosh
	#pragma options align=reset
#endif


#endif /* XAPEX_H */

/*
$Log$
*/

