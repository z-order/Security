// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from OTCDPRecoveryInformation.proto

#import "OTEscrowAuthenticationInformation.h"
#import <ProtocolBuffer/PBConstants.h>
#import <ProtocolBuffer/PBHashUtil.h>
#import <ProtocolBuffer/PBDataReader.h>

#if !__has_feature(objc_arc)
# error This generated file depends on ARC but it is not enabled; turn on ARC, or use 'objc_use_arc' option to generate non-ARC code.
#endif

@implementation OTEscrowAuthenticationInformation

- (BOOL)hasAuthenticationPassword
{
    return _authenticationPassword != nil;
}
@synthesize authenticationPassword = _authenticationPassword;
- (BOOL)hasAuthenticationDsid
{
    return _authenticationDsid != nil;
}
@synthesize authenticationDsid = _authenticationDsid;
- (BOOL)hasAuthenticationAppleid
{
    return _authenticationAppleid != nil;
}
@synthesize authenticationAppleid = _authenticationAppleid;
- (BOOL)hasFmipUuid
{
    return _fmipUuid != nil;
}
@synthesize fmipUuid = _fmipUuid;
@synthesize fmipRecovery = _fmipRecovery;
- (void)setFmipRecovery:(BOOL)v
{
    _has.fmipRecovery = (uint)YES;
    _fmipRecovery = v;
}
- (void)setHasFmipRecovery:(BOOL)f
{
    _has.fmipRecovery = (uint)f;
}
- (BOOL)hasFmipRecovery
{
    return _has.fmipRecovery != 0;
}
@synthesize idmsRecovery = _idmsRecovery;
- (void)setIdmsRecovery:(BOOL)v
{
    _has.idmsRecovery = (uint)YES;
    _idmsRecovery = v;
}
- (void)setHasIdmsRecovery:(BOOL)f
{
    _has.idmsRecovery = (uint)f;
}
- (BOOL)hasIdmsRecovery
{
    return _has.idmsRecovery != 0;
}
- (BOOL)hasAuthenticationAuthToken
{
    return _authenticationAuthToken != nil;
}
@synthesize authenticationAuthToken = _authenticationAuthToken;
- (BOOL)hasAuthenticationEscrowproxyUrl
{
    return _authenticationEscrowproxyUrl != nil;
}
@synthesize authenticationEscrowproxyUrl = _authenticationEscrowproxyUrl;
- (BOOL)hasAuthenticationIcloudEnvironment
{
    return _authenticationIcloudEnvironment != nil;
}
@synthesize authenticationIcloudEnvironment = _authenticationIcloudEnvironment;

- (NSString *)description
{
    return [NSString stringWithFormat:@"%@ %@", [super description], [self dictionaryRepresentation]];
}

- (NSDictionary *)dictionaryRepresentation
{
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if (self->_authenticationPassword)
    {
        [dict setObject:self->_authenticationPassword forKey:@"authentication_password"];
    }
    if (self->_authenticationDsid)
    {
        [dict setObject:self->_authenticationDsid forKey:@"authentication_dsid"];
    }
    if (self->_authenticationAppleid)
    {
        [dict setObject:self->_authenticationAppleid forKey:@"authentication_appleid"];
    }
    if (self->_fmipUuid)
    {
        [dict setObject:self->_fmipUuid forKey:@"fmip_uuid"];
    }
    if (self->_has.fmipRecovery)
    {
        [dict setObject:[NSNumber numberWithBool:self->_fmipRecovery] forKey:@"fmip_recovery"];
    }
    if (self->_has.idmsRecovery)
    {
        [dict setObject:[NSNumber numberWithBool:self->_idmsRecovery] forKey:@"idms_recovery"];
    }
    if (self->_authenticationAuthToken)
    {
        [dict setObject:self->_authenticationAuthToken forKey:@"authentication_auth_token"];
    }
    if (self->_authenticationEscrowproxyUrl)
    {
        [dict setObject:self->_authenticationEscrowproxyUrl forKey:@"authentication_escrowproxy_url"];
    }
    if (self->_authenticationIcloudEnvironment)
    {
        [dict setObject:self->_authenticationIcloudEnvironment forKey:@"authentication_icloud_environment"];
    }
    return dict;
}

BOOL OTEscrowAuthenticationInformationReadFrom(__unsafe_unretained OTEscrowAuthenticationInformation *self, __unsafe_unretained PBDataReader *reader) {
    while (PBReaderHasMoreData(reader)) {
        uint32_t tag = 0;
        uint8_t aType = 0;

        PBReaderReadTag32AndType(reader, &tag, &aType);

        if (PBReaderHasError(reader))
            break;

        if (aType == TYPE_END_GROUP) {
            break;
        }

        switch (tag) {

            case 1 /* authenticationPassword */:
            {
                NSString *new_authenticationPassword = PBReaderReadString(reader);
                self->_authenticationPassword = new_authenticationPassword;
            }
            break;
            case 2 /* authenticationDsid */:
            {
                NSString *new_authenticationDsid = PBReaderReadString(reader);
                self->_authenticationDsid = new_authenticationDsid;
            }
            break;
            case 3 /* authenticationAppleid */:
            {
                NSString *new_authenticationAppleid = PBReaderReadString(reader);
                self->_authenticationAppleid = new_authenticationAppleid;
            }
            break;
            case 4 /* fmipUuid */:
            {
                NSString *new_fmipUuid = PBReaderReadString(reader);
                self->_fmipUuid = new_fmipUuid;
            }
            break;
            case 5 /* fmipRecovery */:
            {
                self->_has.fmipRecovery = (uint)YES;
                self->_fmipRecovery = PBReaderReadBOOL(reader);
            }
            break;
            case 6 /* idmsRecovery */:
            {
                self->_has.idmsRecovery = (uint)YES;
                self->_idmsRecovery = PBReaderReadBOOL(reader);
            }
            break;
            case 7 /* authenticationAuthToken */:
            {
                NSString *new_authenticationAuthToken = PBReaderReadString(reader);
                self->_authenticationAuthToken = new_authenticationAuthToken;
            }
            break;
            case 8 /* authenticationEscrowproxyUrl */:
            {
                NSString *new_authenticationEscrowproxyUrl = PBReaderReadString(reader);
                self->_authenticationEscrowproxyUrl = new_authenticationEscrowproxyUrl;
            }
            break;
            case 9 /* authenticationIcloudEnvironment */:
            {
                NSString *new_authenticationIcloudEnvironment = PBReaderReadString(reader);
                self->_authenticationIcloudEnvironment = new_authenticationIcloudEnvironment;
            }
            break;
            default:
                if (!PBReaderSkipValueWithTag(reader, tag, aType))
                    return NO;
                break;
        }
    }
    return !PBReaderHasError(reader);
}

- (BOOL)readFrom:(PBDataReader *)reader
{
    return OTEscrowAuthenticationInformationReadFrom(self, reader);
}
- (void)writeTo:(PBDataWriter *)writer
{
    /* authenticationPassword */
    {
        if (self->_authenticationPassword)
        {
            PBDataWriterWriteStringField(writer, self->_authenticationPassword, 1);
        }
    }
    /* authenticationDsid */
    {
        if (self->_authenticationDsid)
        {
            PBDataWriterWriteStringField(writer, self->_authenticationDsid, 2);
        }
    }
    /* authenticationAppleid */
    {
        if (self->_authenticationAppleid)
        {
            PBDataWriterWriteStringField(writer, self->_authenticationAppleid, 3);
        }
    }
    /* fmipUuid */
    {
        if (self->_fmipUuid)
        {
            PBDataWriterWriteStringField(writer, self->_fmipUuid, 4);
        }
    }
    /* fmipRecovery */
    {
        if (self->_has.fmipRecovery)
        {
            PBDataWriterWriteBOOLField(writer, self->_fmipRecovery, 5);
        }
    }
    /* idmsRecovery */
    {
        if (self->_has.idmsRecovery)
        {
            PBDataWriterWriteBOOLField(writer, self->_idmsRecovery, 6);
        }
    }
    /* authenticationAuthToken */
    {
        if (self->_authenticationAuthToken)
        {
            PBDataWriterWriteStringField(writer, self->_authenticationAuthToken, 7);
        }
    }
    /* authenticationEscrowproxyUrl */
    {
        if (self->_authenticationEscrowproxyUrl)
        {
            PBDataWriterWriteStringField(writer, self->_authenticationEscrowproxyUrl, 8);
        }
    }
    /* authenticationIcloudEnvironment */
    {
        if (self->_authenticationIcloudEnvironment)
        {
            PBDataWriterWriteStringField(writer, self->_authenticationIcloudEnvironment, 9);
        }
    }
}

- (void)copyTo:(OTEscrowAuthenticationInformation *)other
{
    if (_authenticationPassword)
    {
        other.authenticationPassword = _authenticationPassword;
    }
    if (_authenticationDsid)
    {
        other.authenticationDsid = _authenticationDsid;
    }
    if (_authenticationAppleid)
    {
        other.authenticationAppleid = _authenticationAppleid;
    }
    if (_fmipUuid)
    {
        other.fmipUuid = _fmipUuid;
    }
    if (self->_has.fmipRecovery)
    {
        other->_fmipRecovery = _fmipRecovery;
        other->_has.fmipRecovery = YES;
    }
    if (self->_has.idmsRecovery)
    {
        other->_idmsRecovery = _idmsRecovery;
        other->_has.idmsRecovery = YES;
    }
    if (_authenticationAuthToken)
    {
        other.authenticationAuthToken = _authenticationAuthToken;
    }
    if (_authenticationEscrowproxyUrl)
    {
        other.authenticationEscrowproxyUrl = _authenticationEscrowproxyUrl;
    }
    if (_authenticationIcloudEnvironment)
    {
        other.authenticationIcloudEnvironment = _authenticationIcloudEnvironment;
    }
}

- (id)copyWithZone:(NSZone *)zone
{
    OTEscrowAuthenticationInformation *copy = [[[self class] allocWithZone:zone] init];
    copy->_authenticationPassword = [_authenticationPassword copyWithZone:zone];
    copy->_authenticationDsid = [_authenticationDsid copyWithZone:zone];
    copy->_authenticationAppleid = [_authenticationAppleid copyWithZone:zone];
    copy->_fmipUuid = [_fmipUuid copyWithZone:zone];
    if (self->_has.fmipRecovery)
    {
        copy->_fmipRecovery = _fmipRecovery;
        copy->_has.fmipRecovery = YES;
    }
    if (self->_has.idmsRecovery)
    {
        copy->_idmsRecovery = _idmsRecovery;
        copy->_has.idmsRecovery = YES;
    }
    copy->_authenticationAuthToken = [_authenticationAuthToken copyWithZone:zone];
    copy->_authenticationEscrowproxyUrl = [_authenticationEscrowproxyUrl copyWithZone:zone];
    copy->_authenticationIcloudEnvironment = [_authenticationIcloudEnvironment copyWithZone:zone];
    return copy;
}

- (BOOL)isEqual:(id)object
{
    OTEscrowAuthenticationInformation *other = (OTEscrowAuthenticationInformation *)object;
    return [other isMemberOfClass:[self class]]
    &&
    ((!self->_authenticationPassword && !other->_authenticationPassword) || [self->_authenticationPassword isEqual:other->_authenticationPassword])
    &&
    ((!self->_authenticationDsid && !other->_authenticationDsid) || [self->_authenticationDsid isEqual:other->_authenticationDsid])
    &&
    ((!self->_authenticationAppleid && !other->_authenticationAppleid) || [self->_authenticationAppleid isEqual:other->_authenticationAppleid])
    &&
    ((!self->_fmipUuid && !other->_fmipUuid) || [self->_fmipUuid isEqual:other->_fmipUuid])
    &&
    ((self->_has.fmipRecovery && other->_has.fmipRecovery && ((self->_fmipRecovery && other->_fmipRecovery) || (!self->_fmipRecovery && !other->_fmipRecovery))) || (!self->_has.fmipRecovery && !other->_has.fmipRecovery))
    &&
    ((self->_has.idmsRecovery && other->_has.idmsRecovery && ((self->_idmsRecovery && other->_idmsRecovery) || (!self->_idmsRecovery && !other->_idmsRecovery))) || (!self->_has.idmsRecovery && !other->_has.idmsRecovery))
    &&
    ((!self->_authenticationAuthToken && !other->_authenticationAuthToken) || [self->_authenticationAuthToken isEqual:other->_authenticationAuthToken])
    &&
    ((!self->_authenticationEscrowproxyUrl && !other->_authenticationEscrowproxyUrl) || [self->_authenticationEscrowproxyUrl isEqual:other->_authenticationEscrowproxyUrl])
    &&
    ((!self->_authenticationIcloudEnvironment && !other->_authenticationIcloudEnvironment) || [self->_authenticationIcloudEnvironment isEqual:other->_authenticationIcloudEnvironment])
    ;
}

- (NSUInteger)hash
{
    return 0
    ^
    [self->_authenticationPassword hash]
    ^
    [self->_authenticationDsid hash]
    ^
    [self->_authenticationAppleid hash]
    ^
    [self->_fmipUuid hash]
    ^
    (self->_has.fmipRecovery ? PBHashInt((NSUInteger)self->_fmipRecovery) : 0)
    ^
    (self->_has.idmsRecovery ? PBHashInt((NSUInteger)self->_idmsRecovery) : 0)
    ^
    [self->_authenticationAuthToken hash]
    ^
    [self->_authenticationEscrowproxyUrl hash]
    ^
    [self->_authenticationIcloudEnvironment hash]
    ;
}

- (void)mergeFrom:(OTEscrowAuthenticationInformation *)other
{
    if (other->_authenticationPassword)
    {
        [self setAuthenticationPassword:other->_authenticationPassword];
    }
    if (other->_authenticationDsid)
    {
        [self setAuthenticationDsid:other->_authenticationDsid];
    }
    if (other->_authenticationAppleid)
    {
        [self setAuthenticationAppleid:other->_authenticationAppleid];
    }
    if (other->_fmipUuid)
    {
        [self setFmipUuid:other->_fmipUuid];
    }
    if (other->_has.fmipRecovery)
    {
        self->_fmipRecovery = other->_fmipRecovery;
        self->_has.fmipRecovery = YES;
    }
    if (other->_has.idmsRecovery)
    {
        self->_idmsRecovery = other->_idmsRecovery;
        self->_has.idmsRecovery = YES;
    }
    if (other->_authenticationAuthToken)
    {
        [self setAuthenticationAuthToken:other->_authenticationAuthToken];
    }
    if (other->_authenticationEscrowproxyUrl)
    {
        [self setAuthenticationEscrowproxyUrl:other->_authenticationEscrowproxyUrl];
    }
    if (other->_authenticationIcloudEnvironment)
    {
        [self setAuthenticationIcloudEnvironment:other->_authenticationIcloudEnvironment];
    }
}

@end

