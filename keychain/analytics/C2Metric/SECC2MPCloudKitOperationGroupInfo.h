// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from C2Metric.proto

#import <Foundation/Foundation.h>
#import <ProtocolBuffer/PBCodable.h>

#ifdef __cplusplus
#define SECC2MPCLOUDKITOPERATIONGROUPINFO_FUNCTION extern "C" __attribute__((visibility("hidden")))
#else
#define SECC2MPCLOUDKITOPERATIONGROUPINFO_FUNCTION extern __attribute__((visibility("hidden")))
#endif

__attribute__((visibility("hidden")))
@interface SECC2MPCloudKitOperationGroupInfo : PBCodable <NSCopying>
{
    NSString *_operationGroupId;
    NSString *_operationGroupName;
    BOOL _operationGroupTriggered;
    struct {
        uint operationGroupTriggered:1;
    } _has;
}


@property (nonatomic, readonly) BOOL hasOperationGroupId;
@property (nonatomic, retain) NSString *operationGroupId;

@property (nonatomic, readonly) BOOL hasOperationGroupName;
@property (nonatomic, retain) NSString *operationGroupName;

@property (nonatomic) BOOL hasOperationGroupTriggered;
@property (nonatomic) BOOL operationGroupTriggered;

// Performs a shallow copy into other
- (void)copyTo:(SECC2MPCloudKitOperationGroupInfo *)other;

// Performs a deep merge from other into self
// If set in other, singular values in self are replaced in self
// Singular composite values are recursively merged
// Repeated values from other are appended to repeated values in self
- (void)mergeFrom:(SECC2MPCloudKitOperationGroupInfo *)other;

SECC2MPCLOUDKITOPERATIONGROUPINFO_FUNCTION BOOL SECC2MPCloudKitOperationGroupInfoReadFrom(__unsafe_unretained SECC2MPCloudKitOperationGroupInfo *self, __unsafe_unretained PBDataReader *reader);

@end

