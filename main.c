/*
	LLP64: 4GiB / CH_FACTOR of input data
	LP64: no considerable limits

	dependablec.org
*/
#define EFINE_HELPERS_IMPLEMENTATION
#include "efine_helpers.h"

#define EFINE_FSYNC_IMPLEMENTATION
#include "efine_fsync.h"

#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#if CHAR_BIT > 12
#error "Unsupported platform"
#endif

#define CH_MSG_FAILED_READ_CHAR "Failed to read a character"
#define CH_MSG_FAILED_PRINT "Failed to print"
#define CH_MSG_FAILED_FLUSH "Failed to flush"
#define CH_MSG_FAILED_SYNC "Failed to sync"
#define CH_NR_CHARACTERS ((long)1 << CHAR_BIT)
#define CH_FACTOR 76

EFINE_DEF int ch_read_input_and_count_characters(long *histogram)
{
	int c_int;

	while (EOF != (c_int = efine_fgetc_unlocked_no_eintr(stdin))) {
		assert(0 <= c_int && c_int < CH_NR_CHARACTERS);
		++histogram[c_int];
	}

	if (0 != ferror(stdin)) {
		perror("\n" CH_MSG_FAILED_READ_CHAR);
		return 1;
	}

	return 0;
}

EFINE_DEF long ch_get_max(const long *histogram, ptrdiff_t n)
{
	long hist_max;
	ptrdiff_t i;

	hist_max = 0;
	for (i = 0; i < n; ++i) {
		if (hist_max < histogram[i])
			hist_max = histogram[i];
	}

	return hist_max;
}

EFINE_DEF void ch_multiply_divide_all(long *histogram, long factor,
				      long divisor, ptrdiff_t n)
{
	ptrdiff_t i;

	for (i = 0; i < n; ++i) {
		if (histogram[i] == 0)
			continue;

		histogram[i] *= factor;
		histogram[i] /= divisor;

		if (histogram[i] == 0) {
			histogram[i] = 1;
		}
	}
}

EFINE_DEF int ch_print_histogram(const long *histogram, ptrdiff_t n)
{
	ptrdiff_t i;
	long j;

	for (i = 0; i < n; ++i) {
		if (histogram[i] == 0)
			continue;

		if (isprint(i) && i != '\t') {
			if (0 > fprintf(stdout, "  %c ", (char)i)) {
				perror("\n" CH_MSG_FAILED_PRINT);
				return 1;
			}
		} else {
			if (0 > fprintf(stdout, "%03x ", (int)i)) {
				perror("\n" CH_MSG_FAILED_PRINT);
				return 1;
			}
		}
		for (j = 0; j < histogram[i]; ++j) {
			if (EOF == fputc('#', stdout)) {
				perror("\n" CH_MSG_FAILED_PRINT);
				return 1;
			}
		}
		for (j = histogram[i]; j < CH_FACTOR; ++j) {
			if (EOF == fputc('-', stdout)) {
				perror("\n" CH_MSG_FAILED_PRINT);
				return 1;
			}
		}
		if (EOF == fputc('\n', stdout)) {
			perror("\n" CH_MSG_FAILED_PRINT);
			return 1;
		}
	}

	return 0;
}

EFINE_DEF void ch_fill_long_with(long val, long *p, ptrdiff_t n)
{
	ptrdiff_t i;

	for (i = 0; i < n; ++i)
		p[i] = val;
}

EFINE_DEF int ch_run(void)
{
	long histogram[CH_NR_CHARACTERS];
	long hist_max;

	ch_fill_long_with(0, histogram, CH_NR_CHARACTERS);

	if (0 != ch_read_input_and_count_characters(histogram))
		return 1;

	hist_max = ch_get_max(histogram, CH_NR_CHARACTERS);

	if (hist_max == 0) {
		if (EOF == fputs("\nNo characters\n", stdout)) {
			perror("\n" CH_MSG_FAILED_PRINT);
			return 1;
		}
		goto l_finish;
	}

	ch_multiply_divide_all(histogram, CH_FACTOR, hist_max,
			       CH_NR_CHARACTERS);

	if (0 > fprintf(stdout,
			"\n"
			"Maximum number of characters: %ld\n",
			hist_max)) {
		perror("\n" CH_MSG_FAILED_PRINT);
		return 1;
	}

	if (0 != ch_print_histogram(histogram, CH_NR_CHARACTERS))
		return 1;

l_finish:

	if (0 != fflush(stdout)) {
		perror("\n" CH_MSG_FAILED_FLUSH);
		return 1;
	}

	if (0 != efine_fsync(STDOUT_FILENO)) {
		perror("\n" CH_MSG_FAILED_SYNC);
		return 1;
	}

	return 0;
}

int main(void)
{
	int result;

	flockfile(stdin);
	result = ch_run();
	funlockfile(stdin);

	return result == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
