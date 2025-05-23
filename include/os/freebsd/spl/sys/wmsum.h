// SPDX-License-Identifier: CDDL-1.0
/*
 * CDDL HEADER START
 *
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 *
 * CDDL HEADER END
 */

/*
 * wmsum counters are a reduced version of aggsum counters, optimized for
 * write-mostly scenarios.  They do not provide optimized read functions,
 * but instead allow much cheaper add function.  The primary usage is
 * infrequently read statistic counters, not requiring exact precision.
 *
 * The FreeBSD implementation is directly mapped into counter(9) KPI.
 */

#ifndef	_SYS_WMSUM_H
#define	_SYS_WMSUM_H

#include <sys/types.h>
#include <sys/systm.h>
#include <sys/counter.h>
#include <sys/malloc.h>

#ifdef	__cplusplus
extern "C" {
#endif

#define	wmsum_t	counter_u64_t

static inline void
wmsum_init(wmsum_t *ws, uint64_t value)
{

	*ws = counter_u64_alloc(M_WAITOK);
	counter_u64_add(*ws, value);
}

static inline void
wmsum_fini(wmsum_t *ws)
{

	counter_u64_free(*ws);
}

static inline uint64_t
wmsum_value(wmsum_t *ws)
{

	return (counter_u64_fetch(*ws));
}

static inline void
wmsum_add(wmsum_t *ws, int64_t delta)
{

	counter_u64_add(*ws, delta);
}

#ifdef	__cplusplus
}
#endif

#endif /* _SYS_WMSUM_H */
