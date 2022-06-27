
#ifndef UNCANNYENGINE_VARIABLES_H
#define UNCANNYENGINE_VARIABLES_H


#include <cstdint>


namespace uncanny
{


using i8 = int_fast8_t;
using i16 = int_least16_t;
using i32 = int_fast32_t;
using i64 = int_fast64_t;

using u8 = uint_fast8_t;
using u16 = uint_least16_t;
using u32 = uint_fast32_t;
using u64 = uint_fast64_t;

using f32 = float;
using f64 = double;

using b8 = i8;
using b32 = i32;


static_assert(sizeof(i8) == 1,  "i8 does not match 8 bits");
static_assert(sizeof(i16) == 2, "i16 does not match 16 bits");
static_assert(sizeof(i32) == 4, "i32 does not match 32 bits");
static_assert(sizeof(i64) == 8, "i64 does not match 64 bits");

static_assert(sizeof(u8) == 1,  "u8 does not match 8 bits");
static_assert(sizeof(u16) == 2, "u16 does not match 16 bits");
static_assert(sizeof(u32) == 4, "u32 does not match 32 bits");
static_assert(sizeof(u64) == 8, "u64 does not match 64 bits");

static_assert(sizeof(f32) == 4, "f32 does not match 32 bits");
static_assert(sizeof(f64) == 8, "f64 does not match 64 bits");

static_assert(sizeof(b8) == 1,  "b8 does not match 8 bits");
static_assert(sizeof(b32) == 4, "b32 does not match 32 bits");


#define UFALSE 0
#define UTRUE 1


}


#endif //UNCANNYENGINE_VARIABLES_H
