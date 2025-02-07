// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from OTPairingMessage.proto

#include <stdint.h>
#ifdef __OBJC__
#include <Foundation/Foundation.h>
#endif

#ifndef NS_ENUM
#if (defined(__cplusplus) && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))
#define NS_ENUM(_type, _name) enum _name : _type _name; enum _name : _type
#else
#define NS_ENUM(_type, _name) _type _name; enum
#endif
#endif // !defined(NS_ENUM)

typedef NS_ENUM(int32_t, OTSupportType) {
    OTSupportType_unknown = 0,
    OTSupportType_supported = 1,
    OTSupportType_not_supported = 2,
};
#ifdef __OBJC__
NS_INLINE NSString *OTSupportTypeAsString(OTSupportType value)
{
    switch (value)
    {
        case OTSupportType_unknown: return @"unknown";
        case OTSupportType_supported: return @"supported";
        case OTSupportType_not_supported: return @"not_supported";
        default: return [NSString stringWithFormat:@"(unknown: %i)", value];
    }
}
#endif /* __OBJC__ */
#ifdef __OBJC__
NS_INLINE OTSupportType StringAsOTSupportType(NSString *value)
{
    if ([value isEqualToString:@"unknown"]) return OTSupportType_unknown;
    if ([value isEqualToString:@"supported"]) return OTSupportType_supported;
    if ([value isEqualToString:@"not_supported"]) return OTSupportType_not_supported;
    return OTSupportType_unknown;
}
#endif /* __OBJC__ */
