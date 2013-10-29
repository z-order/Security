/*
 * Copyright (c) 2000-2004 Apple Computer, Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
 
/*
 * SecImportExportPkcs8.cpp - support for generating and parsing/decoding 
 * private keys in PKCS8 format.  
 *
 * The current version (as of March 12 2004) can parse and decode every 
 * PKCS8 blob generated by openssl with the exception of those using 
 * double DES encryption. This has been verified by actually generating 
 * those blobs with openssl and decoding them here. 
 *
 * PLEASE: don't even *think* about changing a single line of code here 
 * without verifying the results against the full import/export regression
 * test in SecurityTests/clxutils/importExport. 
 * 
 */

#include <Security/SecImportExport.h>
#include "SecImportExportPkcs8.h"
#include "SecPkcs8Templates.h"
#include "SecImportExportUtils.h"
#include "SecImportExportCrypto.h"
#include <security_pkcs12/pkcs12Utils.h>
#include <security_pkcs12/pkcs12Crypto.h>
#include <security_asn1/SecNssCoder.h>
#include <Security/keyTemplates.h>	
#include <Security/SecAsn1Templates.h>	
#include <Security/secasn1t.h>
#include <security_asn1/nssUtils.h>
#include <security_utilities/debugging.h>
#include <security_utilities/devrandom.h>
#include <Security/oidsalg.h>
#include <Security/SecKeyPriv.h>
#include <security_cdsa_utils/cuCdsaUtils.h>
#include <openssl/pem.h>
#include <assert.h>
#include <Security/SecBase.h>

#define SecPkcs8Dbg(args...)	secdebug("SecPkcs8", ## args)

#pragma mark --- PKCS5 v1.5 Key Derivation ---

/*
 * PKCS5 v1.5. Caller has gleaned everything except salt, 
 * iterationCount, and IV from the AlgId.algorithm OID.
 *
 * We get salt and iteration count from the incoming alg params.
 * IV is derived along with the unwrapping key from the passphrase. 
 */
static CSSM_RETURN pkcs5_v15_genKey(
	CSSM_CSP_HANDLE			cspHand,
	SecNssCoder				&coder,
	const SecKeyImportExportParameters *keyParams,	
	const CSSM_DATA			&paramData,
	CSSM_ALGORITHMS			keyAlg, 
	CSSM_ALGORITHMS			pbeHashAlg, 
	uint32					keySizeInBits,
	uint32					blockSizeInBytes,
	impExpKeyUnwrapParams   *unwrapParams)
{
	CSSM_KEY				*passKey = NULL;
	CFDataRef				cfPhrase = NULL;
	CSSM_RETURN				crtn;
	OSStatus				ortn;
	CSSM_CRYPTO_DATA		seed;
	CSSM_CC_HANDLE			ccHand = 0;
	CSSM_ACCESS_CREDENTIALS	creds;
	
	
	/* passphrase or passkey? */
	ortn = impExpPassphraseCommon(keyParams, cspHand, SPF_Data, VP_Import,
		(CFTypeRef *)&cfPhrase, &passKey);
	if(ortn) {
		return ortn;
	}
	/* subsequent errors to errOut: */
	
	memset(&seed, 0, sizeof(seed));
	if(cfPhrase != NULL) {
		size_t len = CFDataGetLength(cfPhrase);
		coder.allocItem(seed.Param, len);
		memmove(seed.Param.Data, CFDataGetBytePtr(cfPhrase), len);
		CFRelease(cfPhrase);
	}

	/* hash algorithm --> PBE alg for CSP */
	CSSM_ALGORITHMS		pbeAlg;
	switch(pbeHashAlg) {
		case CSSM_ALGID_MD2:
			pbeAlg = CSSM_ALGID_PKCS5_PBKDF1_MD2;
			break;
		case CSSM_ALGID_MD5:
			pbeAlg = CSSM_ALGID_PKCS5_PBKDF1_MD5;
			break;
		case CSSM_ALGID_SHA1:
			pbeAlg = CSSM_ALGID_PKCS5_PBKDF1_SHA1;
			break;
		default:
			/* really shouldn't happen - pbeHashAlg was inferred by 
			 * pkcsOidToParams() */
			SecPkcs8Dbg("PKCS8: PKCS5 v1/5 bogus hash alg");
			crtn = CSSMERR_CSP_INTERNAL_ERROR;
			goto errOut;
	}
	
	/* Salt and iteration count from alg parameters */
	impExpPKCS5_PBE_Parameters pbeParams;
	memset(&pbeParams, 0, sizeof(pbeParams));
	if(coder.decodeItem(paramData, impExpPKCS5_PBE_ParametersTemplate, &pbeParams)) {
		SecPkcs8Dbg("PKCS8: PKCS5 v1.5 pbeParams decode error");
		crtn = errSecUnknownFormat;
		goto errOut;
	}
	uint32 iterCount;
	if(!p12DataToInt(pbeParams.iterations, iterCount)) {
		SecPkcs8Dbg("PKCS8: bad PKCS5 v1.5 iteration count");
		crtn = errSecUnknownFormat;
		goto errOut;
	}

	/* ask for hard coded 8 bytes of IV */
	coder.allocItem(unwrapParams->iv, 8);
	
	memset(&creds, 0, sizeof(CSSM_ACCESS_CREDENTIALS));
	crtn = CSSM_CSP_CreateDeriveKeyContext(cspHand,
		pbeAlg,
		keyAlg,
		keySizeInBits,
		&creds,
		passKey,		// BaseKey
		iterCount,
		&pbeParams.salt,
		&seed,	
		&ccHand);
	if(crtn) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 CSSM_CSP_CreateDeriveKeyContext failure");
		goto errOut;
	}
	
	memset(unwrapParams->unwrappingKey, 0, sizeof(CSSM_KEY));

	CSSM_DATA		dummyLabel;
	dummyLabel.Data = (uint8 *)"temp unwrap key";
	dummyLabel.Length = strlen((char *)dummyLabel.Data);
	
	crtn = CSSM_DeriveKey(ccHand,
		&unwrapParams->iv,		// IV returned in in/out Param
		CSSM_KEYUSE_ANY,
		/* not extractable even for the short time this key lives */
		CSSM_KEYATTR_RETURN_REF | CSSM_KEYATTR_SENSITIVE,
		&dummyLabel,
		NULL,			// cred and acl
		unwrapParams->unwrappingKey);
	if(crtn) {
		SecPkcs8Dbg("PKCS8: PKCS5 v1.5 CSSM_DeriveKey failure");
	}
errOut:
	if(ccHand != 0) {
		CSSM_DeleteContext(ccHand);
	}
	if(passKey != NULL) {
		CSSM_FreeKey(cspHand, NULL, passKey, CSSM_FALSE);
		free(passKey);
	}
	return crtn;
}

#pragma mark --- PKCS5 v2.0 Key Derivation ---

/*
 * PKCS5 v2.0 has different means of encoding algorithm parameters, 
 * depending on the encryption algorithm.
 */
/*
 * Obtain encryption parameters for PKCS5 v2.0, DES and DES3 variants.
 */
static OSStatus pkcs5_DES_params(
	const CSSM_DATA			&paramData,		// encryptionScheme.parameters
	CSSM_OID				*encrOid,
	impExpKeyUnwrapParams   *unwrapParams,	
	CSSM_ALGORITHMS			*keyAlg,		// RETURNED
	uint32					*keySizeInBits, // IN/OUT (returned if 0 on entry)
	SecNssCoder				&coder)
{
	/* Params is iv as OCTET STRING */
	if(coder.decodeItem(paramData, kSecAsn1OctetStringTemplate, &unwrapParams->iv)) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 DES init vector decode error");
		return errSecUnknownFormat;
	}
	if(nssCompareCssmData(encrOid, &CSSMOID_PKCS5_DES_EDE3_CBC)) {
		*keyAlg = CSSM_ALGID_3DES_3KEY;
		unwrapParams->encrAlg = CSSM_ALGID_3DES_3KEY_EDE;
		if(*keySizeInBits == 0) {
			*keySizeInBits = 3 * 64;
		}
	}
	else {
		*keyAlg = CSSM_ALGID_DES;
		unwrapParams->encrAlg = CSSM_ALGID_DES;
		if(*keySizeInBits == 0) {
			*keySizeInBits = 64;
		}
	}
	unwrapParams->encrPad  = CSSM_PADDING_PKCS7;
	unwrapParams->encrMode = CSSM_ALGMODE_CBCPadIV8;
	return errSecSuccess;
}

/*
 * Obtain encryption parameters for PKCS5 v2.0, RC2 variant. 
 */
static OSStatus pkcs5_RC2_params(
	const CSSM_DATA			&paramData,		// encryptionScheme.parameters
	impExpKeyUnwrapParams   *unwrapParams,	
	CSSM_ALGORITHMS			*keyAlg,		// RETURNED
	uint32					*keySizeInBits, // IN/OUT (returned if 0 on entry)
	SecNssCoder				&coder)
{
	/* Params is impExpPKCS5_RC2Params */
	impExpPKCS5_RC2Params rc2Params;
	memset(&rc2Params, 0, sizeof(rc2Params));
	if(coder.decodeItem(paramData, impExpPKCS5_RC2ParamsTemplate, &rc2Params)) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 RC2 params decode error");
		return errSecUnknownFormat;
	}

	*keyAlg = CSSM_ALGID_RC2;
	unwrapParams->encrAlg  = CSSM_ALGID_RC2;
	unwrapParams->encrPad  = CSSM_PADDING_PKCS7;
	unwrapParams->encrMode = CSSM_ALGMODE_CBCPadIV8;

	/* the version actually maps to effective key size like this */
	/* I swear all of this is in the PKCS5 v2.0 spec */
	unwrapParams->effectiveKeySizeInBits = 32;		// default
	if(rc2Params.version.Data) {
		uint32 v;
		if(!p12DataToInt(rc2Params.version, v)) {
			SecPkcs8Dbg("PKCS8: bad PKCS5 rc2Params.version");
			return errSecUnknownFormat;
		}
		switch(v) {
			case 160:
				unwrapParams->effectiveKeySizeInBits = 40; 
				break;
			case 120: 
				unwrapParams->effectiveKeySizeInBits = 64; 
				break;
			case 58:  
				unwrapParams->effectiveKeySizeInBits = 128; 
				break;
			default:
				if(v >= 256) {
					unwrapParams->effectiveKeySizeInBits = v;
				}
				else {
					/* not in the spec, use as zero */
				}
				break;
		}
	}
	unwrapParams->iv = rc2Params.iv;
	
	/* the PKCS5 spec does not give a default for the RC2 key size */
	if(*keySizeInBits == 0) {
		SecPkcs8Dbg("PKCS8: NO RC2 DEFAULT KEYSIZE!");
		return errSecUnknownFormat;
	}
	return errSecSuccess;
}

/*
 * Infer encryption parameters for PKCS5 v2.0, RC5 variant. 
 * All info contained in encryptionScheme.parameters. 
 */
static OSStatus pkcs5_RC5_params(
	const CSSM_DATA			&paramData,		// encryptionScheme.parameters
	impExpKeyUnwrapParams   *unwrapParams,	
	CSSM_ALGORITHMS			*keyAlg,		// RETURNED
	uint32					*keySizeInBits, // IN/OUT (returned if 0 on entry)
	SecNssCoder				&coder)
{
	/* Params is a impExpPKCS5_RC5Params */
	impExpPKCS5_RC5Params rc5Params;
	memset(&rc5Params, 0, sizeof(rc5Params));
	if(coder.decodeItem(paramData, impExpPKCS5_RC5ParamsTemplate, &rc5Params)) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 RC5 params decode error");
		return errSecUnknownFormat;
	}

	*keyAlg = CSSM_ALGID_RC5;
	unwrapParams->encrAlg  = CSSM_ALGID_RC5;
	unwrapParams->encrPad  = CSSM_PADDING_PKCS7;
	unwrapParams->encrMode = CSSM_ALGMODE_CBCPadIV8;

	if(rc5Params.rounds.Data) {
		if(!p12DataToInt(rc5Params.rounds, unwrapParams->rounds)) {
			SecPkcs8Dbg("PKCS8: bad PKCS5 rc5Params.rounds");
			return errSecUnknownFormat;
		}
	}
	if(rc5Params.blockSizeInBits.Data) {
		if(!p12DataToInt(rc5Params.blockSizeInBits, unwrapParams->blockSizeInBits)) {
			SecPkcs8Dbg("PKCS8: bad PKCS5 rc5Params.blockSizeInBits");
			return errSecUnknownFormat;
		}
	}
	
	/* Spec says default iv is zeroes */
	unwrapParams->iv = rc5Params.iv;
	if(unwrapParams->iv.Length == 0) {
		uint32 len = unwrapParams->blockSizeInBits / 8;
		coder.allocItem(unwrapParams->iv, len);
		memset(unwrapParams->iv.Data, 0, len);
	}
	
	/*
	 * Spec does not give a default for key RC5 size, and openssl doesn't 
	 * support RC5 for PKCS8.
	 */
	if(*keySizeInBits == 0) {
		SecPkcs8Dbg("PKCS8: NO RC5 DEFAULT KEYSIZE!");
		return errSecUnknownFormat;
	}
	return errSecSuccess;
}

/* 
 * Common code to derive a wrap/unwrap key using PBKDF2 (i.e., using PKCS5 v2.0
 * key derivation). Caller must CSSM_FreeKey when done. 
 */
static CSSM_RETURN pbkdf2DeriveKey(
	CSSM_CSP_HANDLE		cspHand,
	SecNssCoder			&coder,
	CSSM_ALGORITHMS		keyAlg,
	uint32				keySizeInBits,
	uint32				iterationCount,
	const CSSM_DATA		&salt,
	const SecKeyImportExportParameters	*keyParams,		// required 
	impExpVerifyPhrase  verifyPhrase,   // for secure passphrase
	CSSM_KEY_PTR		symKey)							// RETURNED
{
	CSSM_KEY					*passKey = NULL;
	CFDataRef					cfPhrase = NULL;
	CSSM_PKCS5_PBKDF2_PARAMS 	pbeParams;
	CSSM_RETURN					crtn;
	OSStatus					ortn;
	CSSM_DATA					dummyLabel;
	CSSM_DATA					pbeData;
	uint32						keyAttr;
	CSSM_CC_HANDLE 				ccHand = 0;
	CSSM_ACCESS_CREDENTIALS		creds;
	
	memset(&pbeParams, 0, sizeof(pbeParams));

	/* passphrase or passkey? */
	ortn = impExpPassphraseCommon(keyParams, cspHand, SPF_Data, verifyPhrase,
		(CFTypeRef *)&cfPhrase, &passKey);
	if(ortn) {
		return ortn;
	}
	/* subsequent errors to errOut: */

	if(cfPhrase != NULL) {
		size_t len = CFDataGetLength(cfPhrase);
		coder.allocItem(pbeParams.Passphrase, len);
		memmove(pbeParams.Passphrase.Data, 
			CFDataGetBytePtr(cfPhrase), len);
		CFRelease(cfPhrase);
	}

	memset(&creds, 0, sizeof(CSSM_ACCESS_CREDENTIALS));
	crtn = CSSM_CSP_CreateDeriveKeyContext(cspHand,
		CSSM_ALGID_PKCS5_PBKDF2,
		keyAlg,
		keySizeInBits,
		&creds,
		passKey,		// BaseKey
		iterationCount,
		&salt,
		NULL,			// seed
		&ccHand);
	if(crtn) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 CSSM_CSP_CreateDeriveKeyContext failure");
		goto errOut;
	}
	
	memset(symKey, 0, sizeof(CSSM_KEY));

	/* not extractable even for the short time this key lives */
	keyAttr = CSSM_KEYATTR_RETURN_REF | CSSM_KEYATTR_SENSITIVE;
	dummyLabel.Data = (uint8 *)"temp unwrap key";
	dummyLabel.Length = strlen((char *)dummyLabel.Data);
	
	pbeParams.PseudoRandomFunction = CSSM_PKCS5_PBKDF2_PRF_HMAC_SHA1;
	pbeData.Data = (uint8 *)&pbeParams;
	pbeData.Length = sizeof(pbeParams);
	crtn = CSSM_DeriveKey(ccHand,
		&pbeData,
		CSSM_KEYUSE_ANY,
		keyAttr,
		&dummyLabel,
		NULL,			// cred and acl
		symKey);
	if(crtn) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 CSSM_DeriveKey failure");
	}
errOut:
	if(ccHand != 0) {
		CSSM_DeleteContext(ccHand);
	}
	if(passKey != NULL) {
		CSSM_FreeKey(cspHand, NULL, passKey, CSSM_FALSE);
		free(passKey);
	}
	return crtn;
}

/* 
 * Obtain PKCS5, v.2.0 key derivation and encryption parameters and 
 * derive the key. This one obtains all of the crypt parameters 
 * from the top-level AlgId.Params. What a mess. 
 */
static CSSM_RETURN pkcs5_v2_genKey(
	CSSM_CSP_HANDLE			cspHand,
	SecNssCoder				&coder,
	const CSSM_DATA			&paramData,
	const SecKeyImportExportParameters *keyParams,
	impExpKeyUnwrapParams   *unwrapParams)
{
	SecPkcs8Dbg("PKCS8: generating PKCS5 v2.0 key");

	CSSM_ALGORITHMS		keyAlg = CSSM_ALGID_NONE;
	uint32				prf = 0;		// CSSM_PKCS5_PBKDF2_PRF_HMAC_SHA1...
	
	/* caller should check */
	assert(keyParams != NULL);

	/* AlgId.Params --> impExpPKCS5_PBES2_Params */
	if(paramData.Length == 0) {
		SecPkcs8Dbg("PKCS8: empty PKCS5 v2 pbes2Params");
		return errSecUnknownFormat;
	}
	impExpPKCS5_PBES2_Params pbes2Params;
	memset(&pbes2Params, 0, sizeof(pbes2Params));
	if(coder.decodeItem(paramData, impExpPKCS5_PBES2_ParamsTemplate, &pbes2Params)) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 pbes2Params decode error");
		return errSecUnknownFormat;
	}
	
	/*
	 * As far as I know the keyDerivationFunc OID must be id-PBKDF2 
	 */
	if(!nssCompareCssmData(&pbes2Params.keyDerivationFunc.algorithm, 
			&CSSMOID_PKCS5_PBKDF2)) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 unexpected keyDerivationFunc alg");
		return errSecUnknownFormat;
	}
	
	/*
	 * The params of the keyDerivationFunc algId are an encoded 
	 * impExpPKCS5_PBKDF2_Params.
	 */
	impExpPKCS5_PBKDF2_Params pbkdf2Params;
	memset(&pbkdf2Params, 0, sizeof(pbkdf2Params));
	if(coder.decodeItem(pbes2Params.keyDerivationFunc.parameters,
			impExpPKCS5_PBKDF2_ParamsTemplate, &pbkdf2Params)) {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 pbkdf2Params decode error");
		return errSecUnknownFormat;
	}
	
	/*
	 * Salt and iteration count from the impExpPKCS5_PBKDF2_Params (ignoring the 
	 * possible CHOICE for salt source).
	 */
	CSSM_DATA salt = pbkdf2Params.salt;
	uint32 iterCount;
	if(!p12DataToInt(pbkdf2Params.iterationCount, iterCount)) {
		SecPkcs8Dbg("PKCS8: bad PKCS5 v2 iteration count");
		return errSecUnknownFormat;
	}
	
	/*
	 * Key size optional, use defaults per alg (later) if it's not there
	 */
	uint32 keySizeInBits = 0;
	if(pbkdf2Params.keyLengthInBytes.Data) {
		uint32 keyLengthInBytes;
		if(!p12DataToInt(pbkdf2Params.keyLengthInBytes, keyLengthInBytes)) {
			SecPkcs8Dbg("PKCS8: bad PKCS5 v2 key size");
			return errSecUnknownFormat;
		}
		keySizeInBits = keyLengthInBytes * 8;
	}
	/* else we'll infer key size from the encryption algorithm */
	
	/* prf optional, but if it's there it better be CSSMOID_PKCS5_HMAC_SHA1 */
	if(pbkdf2Params.prf.Data) {
		if(!nssCompareCssmData(&pbkdf2Params.prf, &CSSMOID_PKCS5_HMAC_SHA1)) {
			SecPkcs8Dbg("PKCS8: PKCS5 v2 unexpected prf OID");
			return errSecUnknownFormat;
		}
	}
	prf = CSSM_PKCS5_PBKDF2_PRF_HMAC_SHA1;

	/*
	 * Now process the encryptionScheme, which is even messier - the algParams
	 * varies per encryption algorithm.
	 */
	CSSM_X509_ALGORITHM_IDENTIFIER &encrScheme = pbes2Params.encryptionScheme;
	CSSM_OID *encrOid = &encrScheme.algorithm;
	OSStatus ortn;
	CSSM_DATA &encrParam = encrScheme.parameters;
	
	if(nssCompareCssmData(encrOid, &CSSMOID_PKCS5_DES_EDE3_CBC) ||
	   nssCompareCssmData(encrOid, &CSSMOID_DES_CBC)) {
		ortn = pkcs5_DES_params(encrParam, encrOid, unwrapParams, &keyAlg, 
			&keySizeInBits, coder);
		if(ortn) {
			return ortn;
		}
	}
	else if(nssCompareCssmData(encrOid, &CSSMOID_PKCS5_RC2_CBC)) {
		ortn = pkcs5_RC2_params(encrParam, unwrapParams, &keyAlg, 
			&keySizeInBits, coder);
		if(ortn) {
			return ortn;
		}
	}
	else if(nssCompareCssmData(encrOid, &CSSMOID_PKCS5_RC5_CBC)) {
		ortn = pkcs5_RC5_params(encrParam, unwrapParams, &keyAlg, 
			&keySizeInBits, coder);
		if(ortn) {
			return ortn;
		}
	}
	else {
		SecPkcs8Dbg("PKCS8: PKCS5 v2 unknown encrScheme.algorithm");
		return errSecUnknownFormat;
	}
	
	/* We should be ready to go */
	assert(keyAlg != CSSM_ALGID_NONE);
	assert(unwrapParams->encrAlg != CSSM_ALGID_NONE);
	
	/* use all the stuff we just figured out to derive a symmetric decryption key */
	return pbkdf2DeriveKey(cspHand, coder,
		keyAlg, keySizeInBits, 
		iterCount, salt,
		keyParams, 
		VP_Import,
		unwrapParams->unwrappingKey);
}

#pragma mark --- PKCS12 Key Derivation ---

/*
 * PKCS12 style key derivation. Caller has gleaned everything except 
 * salt, iterationCount, and IV from the AlgId.algorithm OID.
 *
 * We get salt and iteration count from the incoming alg params.
 * IV is derived along with the unwrapping key from the passphrase. 
 */
static CSSM_RETURN pkcs12_genKey(
	CSSM_CSP_HANDLE		cspHand,
	SecNssCoder			&coder,
	const SecKeyImportExportParameters *keyParams,
	const CSSM_DATA		&paramData,			// from algID
	CSSM_ALGORITHMS		keyAlg,				// valid on entry 
	CSSM_ALGORITHMS		pbeHashAlg,			// valid on entry 
	uint32				keySizeInBits,		// valid on entry
	uint32				blockSizeInBytes,   // for IV
	impExpKeyUnwrapParams  *unwrapParams)
{
	SecPkcs8Dbg("PKCS8: generating PKCS12 key");
	
	assert(keyAlg != CSSM_ALGID_NONE);
	assert(pbeHashAlg != CSSM_ALGID_NONE);
	assert(keySizeInBits != 0);
	
	/* get iteration count, salt from alg params */
	NSS_P12_PBE_Params pbeParams;
	
	if(paramData.Length == 0) {
		SecPkcs8Dbg("PKCS8: empty P12 pbeParams");
		return errSecUnknownFormat;
	}
	memset(&pbeParams, 0, sizeof(pbeParams));
	if(coder.decodeItem(paramData, NSS_P12_PBE_ParamsTemplate, &pbeParams)) {
		SecPkcs8Dbg("PKCS8: P12 pbeParams decode error");
		return errSecUnknownFormat;
	}

	uint32 iterCount = 0;
	if(!p12DataToInt(pbeParams.iterations, iterCount)) {
		SecPkcs8Dbg("PKCS8: bad P12 iteration count");
		return errSecUnknownFormat;
	}
	
	/* passphrase or passkey? */
	CSSM_KEY *passKey = NULL;
	CFStringRef phraseStr = NULL;
	CSSM_DATA phraseData = {0, NULL};
	CSSM_DATA *phraseDataP = NULL;
	OSStatus ortn;
	CSSM_RETURN crtn;
	
	assert(keyParams != NULL);
	
	ortn = impExpPassphraseCommon(keyParams, cspHand, SPF_String, VP_Import,
		(CFTypeRef *)&phraseStr, &passKey);
	if(ortn) {
		return ortn;
	}
	/* subsequent errors to errOut: */
	
	if(phraseStr != NULL) {
		/* convert to CSSM_DATA for use with p12KeyGen() */
		try {
			p12ImportPassPhrase(phraseStr, coder, phraseData);		
		} 
		catch(...) {
			SecPkcs8Dbg("PKCS8: p12ImportPassPhrase threw");
			crtn = errSecAllocate;
			goto errOut;
		}
		CFRelease(phraseStr);
		phraseDataP = &phraseData;
	}   
	
	/* use p12 module to cook up the key and IV */
	if(blockSizeInBytes) {
		coder.allocItem(unwrapParams->iv, blockSizeInBytes);
	}
	crtn = p12KeyGen(cspHand, 
		*unwrapParams->unwrappingKey,
		true,		// isForEncr
		keyAlg,
		pbeHashAlg,
		keySizeInBits,
		iterCount,
		pbeParams.salt,
		phraseDataP,
		passKey,
		unwrapParams->iv);
	if(crtn) {
		SecPkcs8Dbg("PKCS8: p12KeyGen failed");
	}
errOut:
	if(passKey != NULL) {
		CSSM_FreeKey(cspHand, NULL, passKey, CSSM_FALSE);
		free(passKey);
	}
	return crtn;
}

#pragma mark --- Public PKCS8 import function ---

/* 
 * Called out from SecImportRep::importWrappedKey().
 * If cspHand is provided instead of importKeychain, the CSP 
 * handle MUST be for the CSPDL, not for the raw CSP.
 */
OSStatus impExpPkcs8Import(
	CFDataRef							inData,
	SecKeychainRef						importKeychain, // optional
	CSSM_CSP_HANDLE						cspHand,		// required
	SecItemImportExportFlags			flags,
	const SecKeyImportExportParameters	*keyParams,		// REQUIRED for unwrap 
	CFMutableArrayRef					outArray)		// optional, append here 
{
	CSSM_KEY			wrappedKey;
	CSSM_KEYHEADER		&hdr = wrappedKey.KeyHeader;
	CSSM_RETURN			crtn = CSSM_OK;
	
	/* key derivation and encryption parameters gleaned from alg ID */
	impExpKeyUnwrapParams unwrapParams;
	memset(&unwrapParams, 0, sizeof(unwrapParams));
	CSSM_ALGORITHMS		keyAlg = CSSM_ALGID_NONE;
	CSSM_ALGORITHMS		pbeHashAlg = CSSM_ALGID_NONE;	// SHA1 or MD5
	uint32				keySizeInBits;
	uint32				blockSizeInBytes;
	PKCS_Which			pkcs = PW_None;			
	
	if( (keyParams == NULL) ||
	    ( (keyParams->passphrase == NULL) && 
		  !(keyParams->flags & kSecKeySecurePassphrase) ) ) {
		/* passphrase mandatory */
		return errSecPassphraseRequired;
	}
	assert(cspHand != 0);
	
	/*
	 * Top-level decode 
	 */
	SecNssCoder					coder;
	NSS_EncryptedPrivateKeyInfo encrPrivKeyInfo;
	
	memset(&encrPrivKeyInfo, 0, sizeof(encrPrivKeyInfo));
	if(coder.decode(CFDataGetBytePtr(inData),
			CFDataGetLength(inData),
			kSecAsn1EncryptedPrivateKeyInfoTemplate, &encrPrivKeyInfo)) {
		SecImpExpDbg("impExpPkcs8Import: error decoding top-level encrPrivKeyInfo");
		return errSecUnknownFormat;
	}
	
	/*
	 * The algorithm OID of that top-level struct is the key piece of info 
	 * for now...
	 */
	bool found = false;
	found = pkcsOidToParams(&encrPrivKeyInfo.algorithm.algorithm,
		keyAlg, unwrapParams.encrAlg, pbeHashAlg, keySizeInBits, blockSizeInBytes,
		unwrapParams.encrPad, unwrapParams.encrMode, pkcs);
	if(!found) {
		SecImpExpDbg("impExpPkcs8Import: unknown OID in top-level encrPrivKeyInfo");
		return errSecUnknownFormat;
	}
	
	/* 
	 * Each PBE method has its own way of filling in the remaining gaps
	 * in impExpKeyUnwrapParams and generating a key.
	 */
	CSSM_KEY unwrappingKey;
	memset(&unwrappingKey, 0, sizeof(unwrappingKey));
	unwrapParams.unwrappingKey = &unwrappingKey;
	CSSM_DATA &paramData = encrPrivKeyInfo.algorithm.parameters;

	switch(pkcs) {
		case PW_PKCS5_v1_5:
			/* we have everything except iv, iterations, salt */
			crtn = pkcs5_v15_genKey(cspHand, coder, keyParams, paramData,
				keyAlg, pbeHashAlg, keySizeInBits, blockSizeInBytes,
				&unwrapParams);
			break;
			
		case PW_PKCS5_v2:
			/* obtain everything, including iv, from alg params */
			crtn = pkcs5_v2_genKey(cspHand, coder, paramData, keyParams, &unwrapParams);
			break;
		case PW_PKCS12:
			/* we have everything except iv, iterations, salt */
			crtn = pkcs12_genKey(cspHand, coder, keyParams, paramData,
				keyAlg, pbeHashAlg, keySizeInBits, blockSizeInBytes,
				&unwrapParams);
			break;
		case PW_None:
			/* satisfy compiler */
			assert(0);
			return errSecUnknownFormat; 
	}
	if(crtn) {
		SecPkcs8Dbg("PKCS8: key derivation failed");
		return crtn;
	}
	
	/* we should be ready to rock'n'roll no matter how we got here */
	assert(unwrapParams.encrAlg != CSSM_ALGID_NONE);
	assert(unwrappingKey.KeyData.Data != NULL);
	assert(unwrappingKey.KeyHeader.AlgorithmId != CSSM_ALGID_NONE);
	
	/* set up key to unwrap */
	memset(&wrappedKey, 0, sizeof(CSSM_KEY));
	hdr.HeaderVersion = CSSM_KEYHEADER_VERSION;
	/* CspId : don't care */
	hdr.BlobType = CSSM_KEYBLOB_WRAPPED;
	hdr.Format = CSSM_KEYBLOB_WRAPPED_FORMAT_PKCS8;
	/* AlgorithmId : inferred by CSP */
	hdr.AlgorithmId = CSSM_ALGID_NONE;
	hdr.KeyClass = CSSM_KEYCLASS_PRIVATE_KEY;
	/* LogicalKeySizeInBits : calculated by CSP during unwrap */
	hdr.KeyAttr = CSSM_KEYATTR_EXTRACTABLE;
	hdr.KeyUsage = CSSM_KEYUSE_ANY;

	wrappedKey.KeyData = encrPrivKeyInfo.encryptedData;

	crtn = impExpImportKeyCommon(
		&wrappedKey,
		importKeychain,
		cspHand,
		flags,
		keyParams,
		&unwrapParams,
		NULL,			// default label 
		outArray);
	CSSM_FreeKey(cspHand, NULL, &unwrappingKey, CSSM_FALSE);
	return crtn;
}

#pragma mark --- Public PKCS8 export function ---

#define PKCS5_V2_SALT_LEN		8
#define PKCS5_V2_ITERATIONS		2048
#define PKCS5_V2_DES_IV_SIZE	8

/*
 * Unlike impExpPkcs8Import(), which can handle every PBE algorithm in the spec
 * and implemented by openssl, this one has a fixed PBE and encryption scheme. 
 * We do not provide a means at the API for the client to specify these. 
 *
 * We generate blobs with triple DES encryption, with PKCS5 v2.0 key 
 * derivation.
 */
OSStatus impExpPkcs8Export(
	SecKeyRef							secKey,
	SecItemImportExportFlags			flags,		
	const SecKeyImportExportParameters	*keyParams,		// optional 
	CFMutableDataRef					outData,		// output appended here
	const char							**pemHeader)	
{
	DevRandomGenerator				rng;
	SecNssCoder						coder;
	impExpPKCS5_PBES2_Params		pbes2Params;
	CSSM_X509_ALGORITHM_IDENTIFIER  &keyDeriveAlgId = pbes2Params.keyDerivationFunc;
	CSSM_ATTRIBUTE_TYPE				formatAttrType = CSSM_ATTRIBUTE_NONE;
	CSSM_KEYBLOB_FORMAT				blobForm = CSSM_KEYBLOB_RAW_FORMAT_NONE;
	const CSSM_KEY					*cssmKey;
	
	if(keyParams == NULL) {
		return errSecParam;
	}
	assert(secKey != NULL);
	assert(outData != NULL);
	
	memset(&pbes2Params, 0, sizeof(pbes2Params));
	
	/* 
	 * keyDeriveAlgId
	 * parameters is an encoded impExpPKCS5_PBKDF2_Params 
	 * We generate random salt
	 */
	keyDeriveAlgId.algorithm = CSSMOID_PKCS5_PBKDF2;
	impExpPKCS5_PBKDF2_Params pbkdf2Params;
	memset(&pbkdf2Params, 0, sizeof(pbkdf2Params));
	coder.allocItem(pbkdf2Params.salt, PKCS5_V2_SALT_LEN);
	rng.random(pbkdf2Params.salt.Data, PKCS5_V2_SALT_LEN);
	p12IntToData(PKCS5_V2_ITERATIONS, pbkdf2Params.iterationCount, coder);
	/* leave pbkdf2Params.keyLengthInBytes NULL for default */
	/* openssl can't handle this, which is the default value:
	   pbkdf2Params.prf = CSSMOID_PKCS5_HMAC_SHA1;
	 */
	
	coder.encodeItem(&pbkdf2Params, impExpPKCS5_PBKDF2_ParamsTemplate,
			keyDeriveAlgId.parameters);

	/*
	 * encryptionScheme
	 * parameters is an encoded OCTET STRING containing the (random) IV
	 */
	CSSM_X509_ALGORITHM_IDENTIFIER &encrScheme = pbes2Params.encryptionScheme;
	encrScheme.algorithm = CSSMOID_PKCS5_DES_EDE3_CBC;
	CSSM_DATA rawIv = {0, NULL};
	coder.allocItem(rawIv, PKCS5_V2_DES_IV_SIZE);
	rng.random(rawIv.Data, PKCS5_V2_DES_IV_SIZE);

	coder.encodeItem(&rawIv, kSecAsn1OctetStringTemplate,
			encrScheme.parameters);
	
	/*
	 * Top level NSS_EncryptedPrivateKeyInfo, whose parameters is the encoded 
	 * impExpPKCS5_PBES2_Params. 
	 */
	NSS_EncryptedPrivateKeyInfo encrPrivKeyInfo;
	memset(&encrPrivKeyInfo, 0, sizeof(encrPrivKeyInfo));
	CSSM_X509_ALGORITHM_IDENTIFIER &topAlgId = encrPrivKeyInfo.algorithm;
	topAlgId.algorithm = CSSMOID_PKCS5_PBES2;
	coder.encodeItem(&pbes2Params, impExpPKCS5_PBES2_ParamsTemplate,
			topAlgId.parameters);
			
	/*
	 * Now all we have to do is generate the encrypted key data itself. 
	 * When doing a WrapKey op in PKCS8 form, the CSP gives us the 
	 * NSS_EncryptedPrivateKeyInfo.encryptedData values. 
	 */
	
	/* we need a CSPDL handle - try to get it from the key */
	CSSM_CSP_HANDLE cspdlHand = 0;
	OSStatus ortn;
	bool releaseCspHand = false;
	CSSM_DATA encodedKeyInfo = {0, NULL};
	
	ortn = SecKeyGetCSPHandle(secKey, &cspdlHand);
	if(ortn) {
		cspdlHand = cuCspStartup(CSSM_FALSE);
		if(cspdlHand == 0) {
			return CSSMERR_CSSM_ADDIN_LOAD_FAILED;
		}
		releaseCspHand = true;
	}
	/* subsequent errors to errOut: */
	
	/* get wrapping key from parameters we just set up */
	CSSM_KEY wrappingKey;
	memset(&wrappingKey, 0, sizeof(CSSM_KEY));
	CSSM_RETURN crtn = pbkdf2DeriveKey(cspdlHand, coder,
		CSSM_ALGID_3DES_3KEY, 3 * 64, 
		PKCS5_V2_ITERATIONS, pbkdf2Params.salt,
		keyParams,
		VP_Export,
		&wrappingKey);
	if(crtn) {
		goto errOut;
	}

	/*
	 * Special case for DSA, ECDSA: specify that the raw blob, pre-encrypt, is in 
	 * the PKCS8 PrivateKeyInfo format that openssl understands. The
	 * default is BSAFE.
	 */
	crtn = SecKeyGetCSSMKey(secKey, &cssmKey);
	if(crtn) {
		SecImpExpDbg("impExpPkcs8Export SecKeyGetCSSMKey error");
		goto errOut;
	}
	switch(cssmKey->KeyHeader.AlgorithmId) {
		case CSSM_ALGID_DSA:
		case CSSM_ALGID_ECDSA:
			formatAttrType = CSSM_ATTRIBUTE_PRIVATE_KEY_FORMAT;
			blobForm = CSSM_KEYBLOB_RAW_FORMAT_PKCS8;
			break;
		default:
			break;
	}
	
	/* GO */
	CSSM_KEY wrappedKey;
	memset(&wrappedKey, 0, sizeof(CSSM_KEY));
	
	crtn = impExpExportKeyCommon(cspdlHand, secKey, &wrappingKey, &wrappedKey,
		CSSM_ALGID_3DES_3KEY_EDE, CSSM_ALGMODE_CBCPadIV8, CSSM_PADDING_PKCS7,
		CSSM_KEYBLOB_WRAPPED_FORMAT_PKCS8, formatAttrType, blobForm, NULL, &rawIv);
	if(crtn) {
		goto errOut;
	}
	
	/*
	 * OK... *that* wrapped key's data goes into the top-level 
	 * NSS_EncryptedPrivateKeyInfo, which we then encode; the caller
	 * gets the result of that encoding. 
	 */
	encrPrivKeyInfo.encryptedData = wrappedKey.KeyData;
	coder.encodeItem(&encrPrivKeyInfo, kSecAsn1EncryptedPrivateKeyInfoTemplate,
			encodedKeyInfo);
	
	CFDataAppendBytes(outData, encodedKeyInfo.Data, encodedKeyInfo.Length);
	CSSM_FreeKey(cspdlHand, NULL, &wrappedKey, CSSM_FALSE);
	
	*pemHeader = PEM_STRING_PKCS8;
	
errOut:
	if(wrappingKey.KeyData.Data) {
		CSSM_FreeKey(cspdlHand, NULL, &wrappingKey, CSSM_FALSE);
	}
	if(releaseCspHand) {
		cuCspDetachUnload(cspdlHand, CSSM_FALSE);
	}
	return crtn;
}
