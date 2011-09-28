/*
 * $Id$
 *
 * Copyright (c) 2011, Juniper Networks, Inc.
 * All rights reserved.
 * See ../Copyright for the status of this software
 *
 * ext_logical.c -- extension functions for logical operations
 */

#include "slaxinternals.h"
#include <libslax/slax.h>

#include "config.h"

#include <libxml/xpathInternals.h>
#include <libxml/tree.h>
#include <libxslt/extensions.h>
#include <libslax/slaxdyn.h>

#define URI_LOGICAL  "http://xml.juniper.net/extension/logical"

static xmlChar *
extLogicalStringVal (xmlXPathParserContextPtr ctxt, xmlXPathObjectPtr xop)
{
    if (xop->type == XPATH_NUMBER) {
	xmlChar *res;
	int width;
	unsigned long val = xop->floatval, v2;

	xmlXPathFreeObject(xop);

	for (width = 1, v2 = val; v2; width++, v2 /= 2)
	    continue;

	res = xmlMalloc(width + 1);
	res[width] = '\0';
	for (width--, v2 = val; width >= 0; width--, v2 /= 2)
	    res[width] = (v2 & 1) ? '1' : '0';
	
	return res;
    }

    /* Make libxml do the work for us */
    valuePush(ctxt, xop);
    return xmlXPathPopString(ctxt);
}

static int
extLogicalExtractArgs (xmlXPathParserContextPtr ctxt, int nargs,
		       xmlChar **leftp, xmlChar **rightp,
		       int *llen, int *rlen, int *widthp)
{
    xmlXPathObjectPtr xop;

    if (nargs != 2) {
	xmlXPathSetArityError(ctxt);
	return TRUE;
    }

    /* Pop args in reverse order */
    xop = valuePop(ctxt);
    if (xop == NULL || xmlXPathCheckError(ctxt))
	return TRUE;
    *rightp = extLogicalStringVal(ctxt, xop);
    if (*rightp == NULL)
	return TRUE;

    xop = valuePop(ctxt);
    if (xop == NULL || xmlXPathCheckError(ctxt))
	return TRUE;
    *leftp = extLogicalStringVal(ctxt, xop);
    if (*leftp == NULL) {
	xmlFree(*rightp);
	return TRUE;
    }

    *llen = xmlStrlen(*leftp);
    *rlen = xmlStrlen(*rightp);
    *widthp = (*llen > *rlen) ? *llen : *rlen;

    return FALSE;
}

static void
extLogicalAnd (xmlXPathParserContextPtr ctxt, int nargs)
{
    xmlChar *lv, *rv, *res;
    int llen, rlen, width, i;

    if (extLogicalExtractArgs(ctxt, nargs, &lv, &rv,
			      &llen, &rlen, &width))
	return;

    res = xmlMalloc(width + 1);
    if (res) {
	res[width] = '\0';
	for (i = 0; i < width; i++) {
	    xmlChar lb = (i >= width - llen) ? lv[i - (width - llen)] : '0';
	    xmlChar rb = (i >= width - rlen) ? rv[i - (width - rlen)] : '0';
	    res[i] = (lb == '1' && rb == '1') ? '1' : '0';
	}
    }

    slaxLog("logical:and:: %d [%s] & [%s] == [%s]", width, lv, rv, res);

    xmlFree(lv);
    xmlFree(rv);

    xmlXPathReturnString(ctxt, res);
}

static void
extLogicalOr (xmlXPathParserContextPtr ctxt, int nargs)
{
    xmlChar *lv, *rv, *res;
    int llen, rlen, width, i;

    if (extLogicalExtractArgs(ctxt, nargs, &lv, &rv,
			      &llen, &rlen, &width))
	return;

    res = xmlMalloc(width + 1);
    if (res) {
	res[width] = '\0';
	for (i = 0; i < width; i++) {
	    xmlChar lb = (i >= width - llen) ? lv[i - (width - llen)] : '0';
	    xmlChar rb = (i >= width - rlen) ? rv[i - (width - rlen)] : '0';
	    res[i] = (lb == '1' || rb == '1') ? '1' : '0';
	}
    }

    slaxLog("logical:and:: %d [%s] & [%s] == [%s]", width, lv, rv, res);

    xmlFree(lv);
    xmlFree(rv);

    xmlXPathReturnString(ctxt, res);
}

#if 0

template logical:and ( $bitA = "0", $bitB = "0" ) {
	if ($bitA == "1" && $bitB == "1") {
		expr "1";
	}
	else {
		expr "0";
	}
}

template logical:or ( $bitA = "0", $bitB = "0" ) {
	if ($bitA == "1" || $bitB == "1") {
		expr "1";
	}
	else {
		expr "0";
	}
}

template logical:nand ( $bitA = "0", $bitB = "0" ) {
	if ($bitA == "0" && $bitB == "0") {
		expr "1";
	}
	else {
		expr "0";
	}
}

template logical:nor ( $bitA = "0", $bitB = "0" ) {
	if ($bitA == "0" && $bitB == "0") {
		expr "1";
	}
	else {
		expr "0";
	}
}

template logical:xor ( $bitA = "0", $bitB = "0" ) {
	if ($bitA != $bitB) {
		expr "1";
	}
	else {
		expr "0";
	}
}

template logical:xnor ( $bitA = "0", $bitB = "0" ) {
	if ($bitA == $bitB) {
		expr "1";
	}
	else {
		expr "0";
	}
}

template logical:not ( $bitA = "0" ) {
	if ($bitA == "0") {
		expr "1";
	}
	else {
		expr "0";
	}
}



#endif

SLAX_DYN_FUNC(slaxDynLibInit)
{
    slaxRegisterFunction(URI_LOGICAL, "and", extLogicalAnd);
    slaxRegisterFunction(URI_LOGICAL, "or", extLogicalOr);

    return SLAX_DYN_VERSION;
}

SLAX_DYN_FUNC(slaxDynLibClean)
{
    slaxUnregisterFunction(URI_LOGICAL, "or");

    return 0;
}