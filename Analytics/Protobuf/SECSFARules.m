// This file was automatically generated by protocompiler
// DO NOT EDIT!
// Compiled from SFACollection.proto

#import "SECSFARules.h"
#import <ProtocolBuffer/PBConstants.h>
#import <ProtocolBuffer/PBHashUtil.h>
#import <ProtocolBuffer/PBDataReader.h>

#import "SECSFARule.h"

#if !__has_feature(objc_arc)
# error This generated file depends on ARC but it is not enabled; turn on ARC, or use 'objc_use_arc' option to generate non-ARC code.
#endif

@implementation SECSFARules

@synthesize rules = _rules;
- (void)clearRules
{
    [_rules removeAllObjects];
}
- (void)addRules:(SECSFARule *)i
{
    if (!_rules)
    {
        _rules = [[NSMutableArray alloc] init];
    }
    [_rules addObject:i];
}
- (NSUInteger)rulesCount
{
    return [_rules count];
}
- (SECSFARule *)rulesAtIndex:(NSUInteger)idx
{
    return [_rules objectAtIndex:idx];
}
+ (Class)rulesType
{
    return [SECSFARule class];
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"%@ %@", [super description], [self dictionaryRepresentation]];
}

- (NSDictionary *)dictionaryRepresentation
{
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    if ([self->_rules count])
    {
        NSMutableArray *rulesDictReprs = [[NSMutableArray alloc] initWithCapacity:[self->_rules count]];
        for (SECSFARule *i_rules in self->_rules)
        {
            [rulesDictReprs addObject:[i_rules dictionaryRepresentation]];
        }
        [dict setObject:rulesDictReprs forKey:@"rules"];
    }
    return dict;
}

BOOL SECSFARulesReadFrom(__unsafe_unretained SECSFARules *self, __unsafe_unretained PBDataReader *reader) {
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

            case 1 /* rules */:
            {
                SECSFARule *new_rules = [[SECSFARule alloc] init];
                [self addRules:new_rules];
                PBDataReaderMark mark_rules;
                BOOL markError = !PBReaderPlaceMark(reader, &mark_rules);
                if (markError)
                {
                    return NO;
                }
                BOOL inError = !SECSFARuleReadFrom(new_rules, reader);
                if (inError)
                {
                    return NO;
                }
                PBReaderRecallMark(reader, &mark_rules);
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
    return SECSFARulesReadFrom(self, reader);
}
- (void)writeTo:(PBDataWriter *)writer
{
    /* rules */
    {
        for (SECSFARule *i_rules in self->_rules)
        {
            PBDataWriterWriteSubmessage(writer, i_rules, 1);
        }
    }
}

- (void)copyTo:(SECSFARules *)other
{
    if ([self rulesCount])
    {
        [other clearRules];
        NSUInteger rulesCnt = [self rulesCount];
        for (NSUInteger i = 0; i < rulesCnt; i++)
        {
            [other addRules:[self rulesAtIndex:i]];
        }
    }
}

- (id)copyWithZone:(NSZone *)zone
{
    SECSFARules *copy = [[[self class] allocWithZone:zone] init];
    for (SECSFARule *v in _rules)
    {
        SECSFARule *vCopy = [v copyWithZone:zone];
        [copy addRules:vCopy];
    }
    return copy;
}

- (BOOL)isEqual:(id)object
{
    SECSFARules *other = (SECSFARules *)object;
    return [other isMemberOfClass:[self class]]
    &&
    ((!self->_rules && !other->_rules) || [self->_rules isEqual:other->_rules])
    ;
}

- (NSUInteger)hash
{
    return 0
    ^
    [self->_rules hash]
    ;
}

- (void)mergeFrom:(SECSFARules *)other
{
    for (SECSFARule *iter_rules in other->_rules)
    {
        [self addRules:iter_rules];
    }
}

@end

