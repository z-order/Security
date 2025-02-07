/*
 * Copyright (c) 2000-2001,2011-2012,2014 Apple Inc. All Rights Reserved.
 * 
 * The contents of this file constitute Original Code as defined in and are
 * subject to the Apple Public Source License Version 1.2 (the 'License').
 * You may not use this file except in compliance with the License. Please obtain
 * a copy of the License at http://www.apple.com/publicsource and read it before
 * using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. Please see the License for the
 * specific language governing rights and limitations under the License.
 */


/* crypto/rc5/rc5_locl.h */
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */

#include <stdlib.h>

#undef c2l
#define c2l(c,l)	(l =((RC5_32_INT)(*((c)++)))    , \
			 l|=((RC5_32_INT)(*((c)++)))<< 8L, \
			 l|=((RC5_32_INT)(*((c)++)))<<16L, \
			 l|=((RC5_32_INT)(*((c)++)))<<24L)

/* NOTE - c is not incremented as per c2l */
#undef c2ln
#define c2ln(c,l1,l2,n)	{ \
			c+=n; \
			l1=l2=0; \
			switch (n) { \
            case 8: l2 =((RC5_32_INT)(*(--(c))))<<24L; [[fallthrough]]; \
			case 7: l2|=((RC5_32_INT)(*(--(c))))<<16L; [[fallthrough]]; \
			case 6: l2|=((RC5_32_INT)(*(--(c))))<< 8L; [[fallthrough]]; \
			case 5: l2|=((RC5_32_INT)(*(--(c))));      [[fallthrough]]; \
			case 4: l1 =((RC5_32_INT)(*(--(c))))<<24L; [[fallthrough]]; \
			case 3: l1|=((RC5_32_INT)(*(--(c))))<<16L; [[fallthrough]]; \
			case 2: l1|=((RC5_32_INT)(*(--(c))))<< 8L; [[fallthrough]]; \
			case 1: l1|=((RC5_32_INT)(*(--(c))));     \
				} \
			}

#undef l2c
#define l2c(l,c)	(*((c)++)=(unsigned char)(((l)     )&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16L)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>24L)&0xff))

/* NOTE - c is not incremented as per l2c */
#undef l2cn
#define l2cn(l1,l2,c,n)	{ \
			c+=n; \
			switch (n) { \
			case 8: *(--(c))=(unsigned char)(((l2)>>24L)&0xff); [[fallthrough]]; \
			case 7: *(--(c))=(unsigned char)(((l2)>>16L)&0xff); [[fallthrough]]; \
			case 6: *(--(c))=(unsigned char)(((l2)>> 8L)&0xff); [[fallthrough]]; \
			case 5: *(--(c))=(unsigned char)(((l2)     )&0xff); [[fallthrough]]; \
			case 4: *(--(c))=(unsigned char)(((l1)>>24L)&0xff); [[fallthrough]]; \
			case 3: *(--(c))=(unsigned char)(((l1)>>16L)&0xff); [[fallthrough]]; \
			case 2: *(--(c))=(unsigned char)(((l1)>> 8L)&0xff); [[fallthrough]]; \
			case 1: *(--(c))=(unsigned char)(((l1)     )&0xff); \
				} \
			}

/* NOTE - c is not incremented as per n2l */
#define n2ln(c,l1,l2,n)	{ \
			c+=n; \
			l1=l2=0; \
			switch (n) { \
			case 8: l2 =((RC5_32_INT)(*(--(c))))    ; [[fallthrough]]; \
			case 7: l2|=((RC5_32_INT)(*(--(c))))<< 8; [[fallthrough]]; \
			case 6: l2|=((RC5_32_INT)(*(--(c))))<<16; [[fallthrough]]; \
			case 5: l2|=((RC5_32_INT)(*(--(c))))<<24; [[fallthrough]]; \
			case 4: l1 =((RC5_32_INT)(*(--(c))))    ; [[fallthrough]]; \
			case 3: l1|=((RC5_32_INT)(*(--(c))))<< 8; [[fallthrough]]; \
			case 2: l1|=((RC5_32_INT)(*(--(c))))<<16; [[fallthrough]]; \
			case 1: l1|=((RC5_32_INT)(*(--(c))))<<24; \
				} \
			}

/* NOTE - c is not incremented as per l2n */
#define l2nn(l1,l2,c,n)	{ \
			c+=n; \
			switch (n) { \
			case 8: *(--(c))=(unsigned char)(((l2)    )&0xff); [[fallthrough]]; \
			case 7: *(--(c))=(unsigned char)(((l2)>> 8)&0xff); [[fallthrough]]; \
			case 6: *(--(c))=(unsigned char)(((l2)>>16)&0xff); [[fallthrough]]; \
			case 5: *(--(c))=(unsigned char)(((l2)>>24)&0xff); [[fallthrough]]; \
			case 4: *(--(c))=(unsigned char)(((l1)    )&0xff); [[fallthrough]]; \
			case 3: *(--(c))=(unsigned char)(((l1)>> 8)&0xff); [[fallthrough]]; \
			case 2: *(--(c))=(unsigned char)(((l1)>>16)&0xff); [[fallthrough]]; \
			case 1: *(--(c))=(unsigned char)(((l1)>>24)&0xff); \
				} \
			}

#undef n2l
#define n2l(c,l)        (l =((RC5_32_INT)(*((c)++)))<<24L, \
                         l|=((RC5_32_INT)(*((c)++)))<<16L, \
                         l|=((RC5_32_INT)(*((c)++)))<< 8L, \
                         l|=((RC5_32_INT)(*((c)++))))

#undef l2n
#define l2n(l,c)        (*((c)++)=(unsigned char)(((l)>>24L)&0xff), \
                         *((c)++)=(unsigned char)(((l)>>16L)&0xff), \
                         *((c)++)=(unsigned char)(((l)>> 8L)&0xff), \
                         *((c)++)=(unsigned char)(((l)     )&0xff))

#if defined(WIN32) && defined(_MSC_VER)
#define ROTATE_l32(a,n)     _lrotl(a,n)
#define ROTATE_r32(a,n)     _lrotr(a,n)
#else
#define ROTATE_l32(a,n)     (((a)<<(n&0x1f))|(((a)&0xffffffff)>>(32-(n&0x1f))))
#define ROTATE_r32(a,n)     (((a)<<(32-(n&0x1f)))|(((a)&0xffffffff)>>(n&0x1f)))
#endif

#define RC5_32_MASK	0xffffffffL

#define RC5_16_P	0xB7E1
#define RC5_16_Q	0x9E37
#define RC5_32_P	0xB7E15163L
#define RC5_32_Q	0x9E3779B9L
#define RC5_64_P	0xB7E151628AED2A6BLL
#define RC5_64_Q	0x9E3779B97F4A7C15LL

#define E_RC5_32(a,b,s,n) \
	a^=b; \
	a=ROTATE_l32(a,b); \
	a+=s[n]; \
	a&=RC5_32_MASK; \
	b^=a; \
	b=ROTATE_l32(b,a); \
	b+=s[n+1]; \
	b&=RC5_32_MASK;

#define D_RC5_32(a,b,s,n) \
	b-=s[n+1]; \
	b&=RC5_32_MASK; \
	b=ROTATE_r32(b,a); \
	b^=a; \
	a-=s[n]; \
	a&=RC5_32_MASK; \
	a=ROTATE_r32(a,b); \
	a^=b;



