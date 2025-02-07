// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from SFACollection.proto

#import <Foundation/Foundation.h>
#import <ProtocolBuffer/PBCodable.h>

#ifdef __cplusplus
#define SECSFAACTIONTAPTORADAR_FUNCTION extern "C" __attribute__((visibility("hidden")))
#else
#define SECSFAACTIONTAPTORADAR_FUNCTION extern __attribute__((visibility("hidden")))
#endif

/** protocompiler --outputDir Protobuf --arc < ./SFACollection.proto */
__attribute__((visibility("hidden")))
@interface SECSFAActionTapToRadar : PBCodable <NSCopying>
{
    NSString *_alert;
    NSString *_componentID;
    NSString *_componentName;
    NSString *_componentVersion;
    NSString *_radarDescription;
}


@property (nonatomic, readonly) BOOL hasAlert;
@property (nonatomic, retain) NSString *alert;

@property (nonatomic, readonly) BOOL hasRadarDescription;
@property (nonatomic, retain) NSString *radarDescription;

@property (nonatomic, readonly) BOOL hasComponentName;
@property (nonatomic, retain) NSString *componentName;

@property (nonatomic, readonly) BOOL hasComponentVersion;
@property (nonatomic, retain) NSString *componentVersion;

@property (nonatomic, readonly) BOOL hasComponentID;
@property (nonatomic, retain) NSString *componentID;

// Performs a shallow copy into other
- (void)copyTo:(SECSFAActionTapToRadar *)other;

// Performs a deep merge from other into self
// If set in other, singular values in self are replaced in self
// Singular composite values are recursively merged
// Repeated values from other are appended to repeated values in self
- (void)mergeFrom:(SECSFAActionTapToRadar *)other;

SECSFAACTIONTAPTORADAR_FUNCTION BOOL SECSFAActionTapToRadarReadFrom(__unsafe_unretained SECSFAActionTapToRadar *self, __unsafe_unretained PBDataReader *reader);

@end

