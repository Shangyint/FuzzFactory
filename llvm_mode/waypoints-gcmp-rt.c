#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "waypoints.h"

// Debug print statements
#ifdef WAYPOINTS_DEBUG_ON
#define WAYPOINTS_DEBUG(s, ...) printf("pc(0x%08x): ", __wrapgcmp_program_loc); printf(s, __VA_ARGS__)
#else
#define WAYPOINTS_DEBUG(s, ...) 
#endif

// Allocate the the DSF map
FUZZFACTORY_DSF_NEW(__wrapgcmp_dsf_map, MAP_SIZE, FUZZFACTORY_REDUCER_MIN, UINT32_MAX);

// Global variable used to store program location hash
u32 __wrapgcmp_program_loc = 0;

#define BYTE0(x) ((u8) (((x) & 0xff)))
#define BYTE1(x) ((u8) (((x) & 0xff00) >> 8))
#define BYTE2(x) ((u8) (((x) & 0xff0000) >> 16))
#define BYTE3(x) ((u8) (((x) & 0xff000000) >> 24))
#define BYTE4(x) ((u8) (((x) & 0xff00000000LL) >> 32))
#define BYTE5(x) ((u8) (((x) & 0xff0000000000LL) >> 40))
#define BYTE6(x) ((u8) (((x) & 0xff000000000000LL) >> 48))
#define BYTE7(x) ((u8) (((x) & 0xff00000000000000LL) >> 54))

/* Minimize a value `v` at a key given by the current program location hash and current state */
#define WP_KEY()  __wrapgcmp_program_loc
#define WP(v) FUZZFACTORY_DSF_MIN(__wrapcmp_dsf_map, WP_KEY(), v)

/* Increment the location hash; useful when generating new locations adjacent to the randomly generated one */
#define INC_LOC() __wrapgcmp_program_loc++

/* Compare the difference of 8-64 operands `a` and `b`, and set value accordingly */
#define CMP_GT(a, b) \
  do { \
    uint32_t v = a > b ? 0 : b - a + 1; \
    WP(v); \
  } while(0)

#define CMP_GE(a, b) \
  do { \
    uint32_t v = a >= b ? 0 : b - a; \
    WP(v); \
  } while(0)

#define CMP_LT(a, b) \
  do { \
    uint32_t v = a < b ? 0 : a - b + 1; \
    WP(v); \
  } while(0)

#define CMP_LE(a, b) \
  do { \
    uint32_t v = a <= b ? 0 : a - b; \
    WP(v); \
  } while(0)



bool __wrap_gt8(int8_t a, int8_t b);
bool __wrap_gt16(int16_t a, int16_t b);
bool __wrap_gt32(int32_t a, int32_t b);
bool __wrap_gt64(int64_t a, int64_t b);
bool __wrap_ge8(int8_t a, int8_t b);
bool __wrap_ge16(int16_t a, int16_t b);
bool __wrap_ge32(int32_t a, int32_t b);
bool __wrap_ge64(int64_t a, int64_t b);
bool __wrap_lt8(int8_t a, int8_t b);
bool __wrap_lt16(int16_t a, int16_t b);
bool __wrap_lt32(int32_t a, int32_t b);
bool __wrap_lt64(int64_t a, int64_t b);
bool __wrap_le8(int8_t a, int8_t b);
bool __wrap_le16(int16_t a, int16_t b);
bool __wrap_le32(int32_t a, int32_t b);
bool __wrap_le64(int64_t a, int64_t b);



bool __wrap_gt8(int8_t a, int8_t b) {
  CMP_GT(a, b);
  WAYPOINTS_DEBUG("0x%02x > 0x%02x ?\n", a, b);
  return a > b;
}

bool __wrap_gt16(int16_t a, int16_t b) {
  CMP_GT(a, b);
  WAYPOINTS_DEBUG("0x%04x > 0x%04x ?\n", a, b);
  return a > b;
}

bool __wrap_gt32(int32_t a, int32_t b) {
  CMP_GT(a, b);
  WAYPOINTS_DEBUG("0x%08x > 0x%08x ?\n", a, b);
  return a > b;
}

bool __wrap_gt64(int64_t a, int64_t b) {
  CMP_GT(a, b);
  WAYPOINTS_DEBUG("0x%016llx > 0x%016llx ?\n", a, b);
  return a > b;
}

bool __wrap_ge8(int8_t a, int8_t b) {
  CMP_GE(a, b);
  WAYPOINTS_DEBUG("0x%02x >= 0x%02x ?\n", a, b);
  return a >= b;
}

bool __wrap_ge16(int16_t a, int16_t b) {
  CMP_GE(a, b);
  WAYPOINTS_DEBUG("0x%04x >= 0x%04x ?\n", a, b);
  return a >= b;
}

bool __wrap_ge32(int32_t a, int32_t b) {
  CMP_GE(a, b);
  WAYPOINTS_DEBUG("0x%08x >= 0x%08x ?\n", a, b);
  return a >= b;
}

bool __wrap_ge64(int64_t a, int64_t b) {
  CMP_GE(a, b);
  WAYPOINTS_DEBUG("0x%016llx >= 0x%016llx ?\n", a, b);
  return a >= b;
}

bool __wrap_lt8(int8_t a, int8_t b) {
  CMP_LT(a, b);
  WAYPOINTS_DEBUG("0x%02x < 0x%02x ?\n", a, b);
  return a < b;
}

bool __wrap_lt16(int16_t a, int16_t b) {
  CMP_LT(a, b);
  WAYPOINTS_DEBUG("0x%04x < 0x%04x ?\n", a, b);
  return a < b;
}

bool __wrap_lt32(int32_t a, int32_t b) {
  CMP_LT(a, b);
  WAYPOINTS_DEBUG("0x%08x < 0x%08x ?\n", a, b);
  return a < b;
}

bool __wrap_lt64(int64_t a, int64_t b) {
  CMP_LT(a, b);
  WAYPOINTS_DEBUG("0x%016llx < 0x%016llx ?\n", a, b);
  return a < b;
}

bool __wrap_le8(int8_t a, int8_t b) {
  CMP_LE(a, b);
  WAYPOINTS_DEBUG("0x%02x <= 0x%02x ?\n", a, b);
  return a <= b;
}

bool __wrap_le16(int16_t a, int16_t b) {
  CMP_LE(a, b);
  WAYPOINTS_DEBUG("0x%04x <= 0x%04x ?\n", a, b);
  return a <= b;
}

bool __wrap_le32(int32_t a, int32_t b) {
  CMP_LE(a, b);
  WAYPOINTS_DEBUG("0x%08x <= 0x%08x ?\n", a, b);
  return a <= b;
}

bool __wrap_le64(int64_t a, int64_t b) {
  CMP_LE(a, b);
  WAYPOINTS_DEBUG("0x%016llx <= 0x%016llx ?\n", a, b);
  return a <= b;
}

