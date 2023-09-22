// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from OTPairingMessage.proto

#import <Foundation/Foundation.h>
#import <ProtocolBuffer/PBCodable.h>

#import "OTGlobalEnums.h"

#ifdef __cplusplus
#define OTSUPPORTOCTAGONMESSAGE_FUNCTION extern "C" __attribute__((visibility("hidden")))
#else
#define OTSUPPORTOCTAGONMESSAGE_FUNCTION extern __attribute__((visibility("hidden")))
#endif

__attribute__((visibility("hidden")))
@interface OTSupportOctagonMessage : PBCodable <NSCopying>
{
    OTSupportType _supported;
    struct {
        uint supported:1;
    } _has;
}


@property (nonatomic) BOOL hasSupported;
@property (nonatomic) OTSupportType supported;
- (NSString *)supportedAsString:(OTSupportType)value;
- (OTSupportType)StringAsSupported:(NSString *)str;

// Performs a shallow copy into other
- (void)copyTo:(OTSupportOctagonMessage *)other;

// Performs a deep merge from other into self
// If set in other, singular values in self are replaced in self
// Singular composite values are recursively merged
// Repeated values from other are appended to repeated values in self
- (void)mergeFrom:(OTSupportOctagonMessage *)other;

OTSUPPORTOCTAGONMESSAGE_FUNCTION BOOL OTSupportOctagonMessageReadFrom(__unsafe_unretained OTSupportOctagonMessage *self, __unsafe_unretained PBDataReader *reader);

@end

