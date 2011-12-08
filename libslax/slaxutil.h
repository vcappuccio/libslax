/*
 * $Id$
 *
 * Copyright (c) 2010, Juniper Networks, Inc.
 * All rights reserved.
 * See ../Copyright for the status of this software
 */

#ifndef LIBSLAX_SLAXUTIL_H
#define LIBSLAX_SLAXUTIL_H

#include <sys/time.h>
#include <ctype.h>

/* ----------------------------------------------------------------------
 * Functions that work on generic strings
 */

#ifndef HAVE_STREQ
/**
 * @brief
 * Given two strings, return true if they are the same.
 * 
 * Tests the first characters for equality before calling strcmp.
 *
 * @param[in] red, blue
 *     The strings to be compared
 *
 * @return
 *     Nonzero (true) if the strings are the same. 
 */
static inline int
streq (const char *red, const char *blue)
{
    return (red && blue && *red == *blue && strcmp(red + 1, blue + 1) == 0);
}
#endif /* HAVE_STREQ */

#ifndef HAVE_CONST_DROP

/*
 * See const_* method below
 */
typedef union {
    void        *cg_vp;
    const void  *cg_cvp;
} const_remove_glue_t;

/*
 * NOTE:
 *     This is EVIL.  The ONLY time you cast from a const is when calling some
 *     legacy function that does not allow const and:
 *          - you KNOW it does not modify the data
 *          - you can't change it
 *     That is why this is provided.  In that situation, the legacy API should
 *     have been written to accept const argument.  If you can change the 
 *     function you are calling to accept const, do THAT and DO NOT use this
 *     HACK.
 */
static inline void *
const_drop (const void *ptr)
{
    const_remove_glue_t cg;
    cg.cg_cvp = ptr;
    return cg.cg_vp;
}

#endif /* HAVE_CONST_DROP */

#ifndef ALLOCADUP
/**
 * ALLOCADUP(): think of is as strdup + alloca
 *
 * @to destination string
 * @from source string
 */
static inline char *
allocadupx (char *to, const char *from)
{
    if (to) strcpy(to, from);
    return to;
}
#define ALLOCADUP(s) allocadupx((char *) alloca(strlen(s) + 1), s)
#define ALLOCADUPX(s) \
    ((s) ? allocadupx((char *) alloca(strlen(s) + 1), s) : NULL)

#endif /* ALLOCADUP */

/*
 * A "safe" snprintf that never eats more than it can handle.
 */
#define SNPRINTF(_start, _end, _fmt...) \
    do { \
	(_start) += snprintf((_start), (_end) - (_start), _fmt); \
	if ((_start) > (_end)) \
	    (_start) = (_end); \
    } while (0)

/* ---------------------------------------------------------------------- */

/*
 * A type for holding micro seconds.  This is for comparisons, so the fact
 * that we leak seconds off the top is not a concern.
 */
typedef unsigned long time_usecs_t;

static inline time_usecs_t
timeval_to_usecs (struct timeval *tvp)
{
    return tvp->tv_sec * 1000000 + tvp->tv_usec;
}

/* ---------------------------------------------------------------------- */

#ifndef HAVE_STRLCPY

static inline size_t
strlcpy (char *dst, const char *src, size_t sz)
{
    size_t len = strlen(src);

    if (sz > len)
        sz = len;
    memmove(dst, src, sz);
    dst[sz] = '\0';

    return len;
}

#endif /* HAVE_STRLCPY */

/* ---------------------------------------------------------------------- */

/* Useful time-related constants */
#define NSEC_PER_SEC 1000000000
#define NSEC_PER_MSEC 1000000
#define NSEC_PER_USEC 1000
#define USEC_PER_SEC 1000000
#define USEC_PER_MSEC 1000
#define MSEC_PER_SEC 1000

/* ---------------------------------------------------------------------- */

#ifndef HAVE_STRERROR

static inline const char *
strerror (int num)
{
    return "unknown error";
}

#endif /* HAVE_STRERROR */

/*
 * ISO compatible replacement for ctime(3).
 * NOTE: takes time_t by reference, not value.
 */
static inline char *
slaxTimeIso (const time_t *t, char *buf, size_t bufsiz)
{
    if (strftime(buf, bufsiz, "%Y-%m-%d %T %Z", localtime(t)) == 0)
        return NULL;

    return buf;
}

#ifndef HAVE_ASPRINTF

/*
 * stdio.h has already declared asprintf() as non-static.  We use a
 * quick #define to get around that.
 */
#define asprintf slaxAsprintf

static inline int
slaxAsprintf (char **ret, const char *format, ...)
{
    char buf[128], *bp;		/* Smallish buffer */
    int rc;
    va_list vap;

    va_start(vap, format);
    rc = vsnprintf(buf, sizeof(buf), format, vap);
    if (rc < (int) sizeof(buf)) {
	*ret = bp = xmlStrdup2(buf);

    } else {
	*ret = bp = xmlMalloc(rc + 1);
	if (bp == NULL)
	    rc = -1;
	else
	    vsnprintf(bp, rc + 1, format, vap);
    }

    va_end(vap);
    return rc;
}

#endif /* HAVE_ASPRINTF */

#endif /* LIBSLAX_SLAXUTIL_H */
