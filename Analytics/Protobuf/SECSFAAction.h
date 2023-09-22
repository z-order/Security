// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from SFACollection.proto

#import <Foundation/Foundation.h>
#import <ProtocolBuffer/PBCodable.h>

@class SECSFAActionAutomaticBugCapture;
@class SECSFAActionDropEvent;
@class SECSFAActionTapToRadar;

typedef NS_ENUM(int32_t, SECSFAAction_Action) {
    SECSFAAction_Action_PBUNSET = 0,
    SECSFAAction_Action_Ttr = 1,
    SECSFAAction_Action_Abc = 2,
    SECSFAAction_Action_Drop = 3,
};
#ifdef __OBJC__
NS_INLINE NSString *SECSFAAction_ActionAsString(SECSFAAction_Action value)
{
    switch (value)
    {
        case SECSFAAction_Action_PBUNSET: return @"PBUNSET";
        case SECSFAAction_Action_Ttr: return @"ttr";
        case SECSFAAction_Action_Abc: return @"abc";
        case SECSFAAction_Action_Drop: return @"drop";
        default: return [NSString stringWithFormat:@"(unknown: %i)", value];
    }
}
#endif /* __OBJC__ */
#ifdef __OBJC__
NS_INLINE SECSFAAction_Action StringAsSECSFAAction_Action(NSString *value)
{
    if ([value isEqualToString:@"PBUNSET"]) return SECSFAAction_Action_PBUNSET;
    if ([value isEqualToString:@"ttr"]) return SECSFAAction_Action_Ttr;
    if ([value isEqualToString:@"abc"]) return SECSFAAction_Action_Abc;
    if ([value isEqualToString:@"drop"]) return SECSFAAction_Action_Drop;
    return SECSFAAction_Action_PBUNSET;
}
#endif /* __OBJC__ */

#ifdef __cplusplus
#define SECSFAACTION_FUNCTION extern "C" __attribute__((visibility("hidden")))
#else
#define SECSFAACTION_FUNCTION extern __attribute__((visibility("hidden")))
#endif

__attribute__((visibility("hidden")))
@interface SECSFAAction : PBCodable <NSCopying>
{
    SECSFAActionAutomaticBugCapture *_abc;
    SECSFAAction_Action _action;
    SECSFAActionDropEvent *_drop;
    NSString *_radarnumber;
    SECSFAActionTapToRadar *_ttr;
    struct {
        uint action:1;
    } _has;
}


- (void)clearOneofValuesForAction;
@property (nonatomic, readonly) BOOL hasRadarnumber;
@property (nonatomic, retain) NSString *radarnumber;

@property (nonatomic, readonly) BOOL hasTtr;
@property (nonatomic, retain) SECSFAActionTapToRadar *ttr;

@property (nonatomic, readonly) BOOL hasAbc;
@property (nonatomic, retain) SECSFAActionAutomaticBugCapture *abc;

@property (nonatomic, readonly) BOOL hasDrop;
@property (nonatomic, retain) SECSFAActionDropEvent *drop;

@property (nonatomic) BOOL hasAction;
@property (nonatomic) SECSFAAction_Action action;
- (NSString *)actionAsString:(SECSFAAction_Action)value;
- (SECSFAAction_Action)StringAsAction:(NSString *)str;

// Performs a shallow copy into other
- (void)copyTo:(SECSFAAction *)other;

// Performs a deep merge from other into self
// If set in other, singular values in self are replaced in self
// Singular composite values are recursively merged
// Repeated values from other are appended to repeated values in self
- (void)mergeFrom:(SECSFAAction *)other;

SECSFAACTION_FUNCTION BOOL SECSFAActionReadFrom(__unsafe_unretained SECSFAAction *self, __unsafe_unretained PBDataReader *reader);

@end

