/*
	Helpers to make C more dependable
*/

#if !defined(EFINE_HELPERS_H_SENTRY)
#define EFINE_HELPERS_H_SENTRY
#include <assert.h>

/* static or extern */
#if !defined(EFINE_DEF)
#define EFINE_DEF static
#endif

/* returns 0 or 1 */
#define EFINE_DO_NOT_CARE_0_OR_1() 0

/* sar instruction */
#define EFINE_FLOOR_DIV_BY_POW_2(type, x, n)                                                                           \
	(((x) > -1) ? ((x) / ((type)1 << (n))) : (-((-((x) + 1)) / ((type)1 << (n))) - 1))

/* FILE* stream */
EFINE_DEF int efine_fgetc_unlocked_no_eintr(void *stream);

/* FILE* stream */
EFINE_DEF int efine_fputc_unlocked_no_eintr(int c, void *stream);

/* FILE* stream */
EFINE_DEF int efine_fgetc_no_eintr(void *stream);

/* FILE* stream */
EFINE_DEF int efine_fputc_no_eintr(int c, void *stream);

#define EFINE_ASSUME(x)                                                                                                \
	do {                                                                                                           \
		if (EFINE_NDEBUG) {                                                                                    \
			if (!(x)) {                                                                                    \
				__builtin_unreachable();                                                               \
			}                                                                                              \
		} else {                                                                                               \
			assert(x);                                                                                     \
		}                                                                                                      \
	} while (0 == 1)

#if defined(EFINE_HELPERS_IMPLEMENTATION)
#include "efine_portability_base.h"
#include <errno.h>
#include <stdio.h>

EFINE_DEF int efine_fgetc_unlocked_no_eintr(void *stream)
{
	int c_int;

	do {
		c_int = fgetc_unlocked((FILE *)stream);
	} while (c_int == EOF && errno == EINTR);

	return c_int;
}

EFINE_DEF int efine_fputc_unlocked_no_eintr(int c, void *stream)
{
	int result;

	do {
		result = fputc_unlocked(c, (FILE *)stream);
	} while (result == EOF && errno == EINTR);

	return result;
}

EFINE_DEF int efine_fgetc_no_eintr(void *stream)
{
	int c_int;

	do {
		c_int = fgetc((FILE *)stream);
	} while (c_int == EOF && errno == EINTR);

	return c_int;
}

EFINE_DEF int efine_fputc_no_eintr(int c, void *stream)
{
	int result;

	do {
		result = fputc(c, (FILE *)stream);
	} while (result == EOF && errno == EINTR);

	return result;
}

#endif /* EFINE_HELPERS_IMPLEMENTATION */

#endif /* EFINE_HELPERS_H_SENTRY */

/* 2026-04-13 */
