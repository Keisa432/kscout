/*
 * kscout_error.h
 *
 * Common error codes for the kscout library.
 * All functions return int; negative values indicate failure.
 * Use kscout_strerror() to obtain a human-readable description.
 */

#ifndef KSCOUT_ERROR_H
#define KSCOUT_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------
 * Error codes
 * ---------------------------------------------------------------------- */

typedef enum {
    /* Success */
    KSCOUT_OK                   =  0,

    /* Generic / unclassified failure */
    KSCOUT_ERR_UNKNOWN          = -1,

    /* Bad argument passed to a function (NULL pointer, out-of-range value) */
    KSCOUT_ERR_INVALID          = -2,

    /* Memory allocation failed */
    KSCOUT_ERR_OOM              = -3,

    /* Index or offset is out of valid bounds */
    KSCOUT_ERR_OUT_OF_BOUNDS    = -4,

    /* Requested resource or entry was not found */
    KSCOUT_ERR_NOT_FOUND        = -5,

    /* Entity already exists / duplicate key */
    KSCOUT_ERR_ALREADY_EXISTS   = -6,

    /* Buffer too small to hold the result */
    KSCOUT_ERR_BUFFER_TOO_SMALL = -7,

    /* Input data could not be parsed or is malformed */
    KSCOUT_ERR_PARSE            = -8,

    /* Data failed a validation or consistency check */
    KSCOUT_ERR_INVALID_DATA     = -9,

    /* Operation not supported in the current context or configuration */
    KSCOUT_ERR_NOT_SUPPORTED    = -10,

    /* Object or resource has not been initialised */
    KSCOUT_ERR_NOT_INITIALIZED  = -11,

    /* Object or resource has already been initialised */
    KSCOUT_ERR_ALREADY_INITIALIZED = -12,

    /* Underlying I/O operation failed */
    KSCOUT_ERR_IO               = -13,

    /* File or path not found */
    KSCOUT_ERR_FILE_NOT_FOUND   = -14,

    /* Permission denied */
    KSCOUT_ERR_PERMISSION       = -15,

    /* Operation timed out */
    KSCOUT_ERR_TIMEOUT          = -16,

    /* Resource is busy or locked */
    KSCOUT_ERR_BUSY             = -17,

    /* Container or collection is empty */
    KSCOUT_ERR_EMPTY            = -18,

    /* Container or collection is full */
    KSCOUT_ERR_FULL             = -19,

    /* Operation was explicitly cancelled */
    KSCOUT_ERR_CANCELLED        = -20,

    /* Sentinel — keep last */
    KSCOUT_ERR_COUNT_           = -21
} kscout_error_t;

/* -------------------------------------------------------------------------
 * Convenience macros
 * ---------------------------------------------------------------------- */

/** Evaluates to 1 if err indicates success, 0 otherwise. */
#define KSCOUT_IS_OK(err)   ((err) == KSCOUT_OK)

/** Evaluates to 1 if err indicates any failure, 0 otherwise. */
#define KSCOUT_IS_ERR(err)  ((err) < KSCOUT_OK)

/* -------------------------------------------------------------------------
 * kscout_strerror
 *
 * Returns a static, human-readable string for the given error code.
 * Never returns NULL.
 * ---------------------------------------------------------------------- */
static inline const char *kscout_strerror(kscout_error_t err)
{
    switch (err) {
        case KSCOUT_OK:                     return "success";
        case KSCOUT_ERR_UNKNOWN:            return "unknown error";
        case KSCOUT_ERR_INVALID:            return "invalid argument";
        case KSCOUT_ERR_OOM:                return "out of memory";
        case KSCOUT_ERR_OUT_OF_BOUNDS:      return "index out of bounds";
        case KSCOUT_ERR_NOT_FOUND:          return "not found";
        case KSCOUT_ERR_ALREADY_EXISTS:     return "already exists";
        case KSCOUT_ERR_BUFFER_TOO_SMALL:   return "buffer too small";
        case KSCOUT_ERR_PARSE:              return "parse error";
        case KSCOUT_ERR_INVALID_DATA:       return "invalid data";
        case KSCOUT_ERR_NOT_SUPPORTED:      return "not supported";
        case KSCOUT_ERR_NOT_INITIALIZED:    return "not initialized";
        case KSCOUT_ERR_ALREADY_INITIALIZED:return "already initialized";
        case KSCOUT_ERR_IO:                 return "I/O error";
        case KSCOUT_ERR_FILE_NOT_FOUND:     return "file not found";
        case KSCOUT_ERR_PERMISSION:         return "permission denied";
        case KSCOUT_ERR_TIMEOUT:            return "operation timed out";
        case KSCOUT_ERR_BUSY:               return "resource busy";
        case KSCOUT_ERR_EMPTY:              return "collection is empty";
        case KSCOUT_ERR_FULL:              return "collection is full";
        case KSCOUT_ERR_CANCELLED:          return "operation cancelled";
        default:                            return "unrecognised error code";
    }
}

#ifdef __cplusplus
}
#endif

#endif /* KSCOUT_ERROR_H */