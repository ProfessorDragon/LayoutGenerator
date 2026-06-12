#pragma once

/// commonly used object ids
class ObjectId
{
public:
    static const int BLOCK = 1;
    static const int BLOCK_LINE = 2;
    static const int SPIKE = 8;
    static const int JUMP_INDICATOR_FLYING = 236;
    static const int GAMEMODE_PORTAL_WAVE = 660;
    static const int TEXT = 914;
    static const int SLOPE = 1743;
    static const int SLOPE_GENTLE = 1744;
    static const int D_BLOCK = 1755;
    static const int TRAIL_INDICATOR = 1764;
    static const int TRAIL_INDICATOR_CLICKING = 1765;
    static const int TRAIL_INDICATOR_PLACING = 1767;
    static const int JUMP_INDICATOR_GROUNDED = 1768;
    static const int S_BLOCK = 1829;
};

enum PoolTag
{
    // block type
    BLOCK = 1 << 0,
    BREAKABLE_BLOCK = 1 << 1,
    PAD = 1 << 2,
    RING_BUFFERED = 1 << 3,
    RING_LATE = 1 << 4,
    RING = RING_BUFFERED | RING_LATE,
    PORTAL = 1 << 5,

    // what the block does
    JUMP = 1 << 6,      // sends player upwards
    JUMP_HIGH = 1 << 7, // sends player upwards a lot (red pad/orb)
    FALL = 1 << 8,      // sends player downwards
    GRAVITY = 1 << 9,   // changes player gravity
    SIZE_ = 1 << 10,    // changes player size
    SPEED = 1 << 11,    // changes player speed
    GAMEMODE = 1 << 12, // changes gamemode
    SPIDER = 1 << 13,   // teleports player to ground
    BUFFERED = 1 << 14, // can only be interacted with when a tap is buffered
};

enum PoolState
{
    NONE = 0,

    GROUNDED = 1 << 0,
    AIRBORNE = 1 << 1,
    // unused
    // GROUNDED_FLOOR = 1 << 2,
    // GROUNDED_CEILING = 1 << 3,

    RISING = 1 << 4,
    PEAKING = 1 << 5,
    FALLING = 1 << 6,

    GRAVITY_NORMAL = 1 << 7,
    GRAVITY_REVERSE = 1 << 8,

    SIZE_NORMAL = 1 << 9,
    SIZE_MINI = 1 << 10,

    SPEED_SLOW = 1 << 11,
    SPEED_NORMAL = 1 << 12,
    SPEED_2 = 1 << 13,
    SPEED_3 = 1 << 14,
    SPEED_4 = 1 << 15,

    GAMEMODE_CUBE = 1 << 16,
    GAMEMODE_SHIP = 1 << 17,
    GAMEMODE_BALL = 1 << 18,
    GAMEMODE_UFO = 1 << 19,
    GAMEMODE_WAVE = 1 << 20,
    GAMEMODE_ROBOT = 1 << 21,
    GAMEMODE_SPIDER = 1 << 22,
    GAMEMODE_SWING = 1 << 23,

    FLYING = GAMEMODE_SHIP | GAMEMODE_UFO | GAMEMODE_WAVE | GAMEMODE_SWING,
    TAP_FLYING = GAMEMODE_UFO | GAMEMODE_SWING,
    HOLD_FLYING = GAMEMODE_SHIP | GAMEMODE_WAVE,
    NOT_FLYING = GAMEMODE_CUBE | GAMEMODE_BALL | GAMEMODE_ROBOT | GAMEMODE_SPIDER,
    NOT_WAVE = GAMEMODE_CUBE | GAMEMODE_SHIP | GAMEMODE_BALL | GAMEMODE_UFO | GAMEMODE_ROBOT | GAMEMODE_SPIDER | GAMEMODE_SWING,
    NOT_SPIDER = GAMEMODE_CUBE | GAMEMODE_SHIP | GAMEMODE_BALL | GAMEMODE_UFO | GAMEMODE_WAVE | GAMEMODE_ROBOT | GAMEMODE_SWING,
    HAS_BOUNDS = GAMEMODE_SHIP | GAMEMODE_BALL | GAMEMODE_UFO | GAMEMODE_WAVE | GAMEMODE_SPIDER | GAMEMODE_SWING,
    NO_BOUNDS = GAMEMODE_CUBE | GAMEMODE_ROBOT,
    JUMP_NEEDS_BUFFER = GAMEMODE_BALL | GAMEMODE_ROBOT | GAMEMODE_SPIDER,
};

enum PoolAlign
{
    TL = 1 << 0,
    TC = 1 << 1,
    TR = 1 << 2,
    CL = 1 << 3,
    CC = 1 << 4,
    CR = 1 << 5,
    BL = 1 << 6,
    BC = 1 << 7,
    BR = 1 << 8,

    T = TL | TC | TR,
    B = BL | BC | BR,
    L = TL | CL | BL,
    R = TR | CR | BR,
};

enum PoolTap
{
    NO = 1 << 0,
    TAP = 1 << 1,
    TAP_DELAYED = 1 << 2, // tap, but a bit late
    HOLD = 1 << 3,        // hold until the next object is placed
    HOLD_RANDOM = 1 << 4, // hold for a random duration (robot)
    ANY = 1 << 5,         // doesn't matter if you tap, the player can't do anything
    RANDOM = 1 << 6,      // mash randomly

    TAP_OR_HOLD = TAP | TAP_DELAYED | HOLD | HOLD_RANDOM,
};
