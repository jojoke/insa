/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rdict.h"

bool_t
xdr_DictRecord (XDR *xdrs, DictRecord *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->longueur))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->donnee, MAXSTRING1,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_DictName (XDR *xdrs, DictName *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_int (xdrs, &objp->longueur))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->donnee, MAXSTRING1,
		sizeof (char), (xdrproc_t) xdr_char))
		 return FALSE;
	return TRUE;
}
