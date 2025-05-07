/*
 * opencog/atoms/flow/IncrementValueLink.h
 *
 * Copyright (C) 2020, 2025 Linas Vepstas
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_INCREMENT_VALUE_LINK_H
#define _OPENCOG_INCREMENT_VALUE_LINK_H

#include <opencog/atoms/core/FunctionLink.h>

namespace opencog
{
/** \addtogroup grp_atomspace
 *  @{
 */

/// The IncrementValueLink increments a FloatValue located on the
/// indicated atom (first argument), at the indicated key (second
/// argument) by the FloatValue (or NumberNode) given in the third
/// argument.
///
class IncrementValueLink : public FunctionLink
{
public:
	IncrementValueLink(const HandleSeq&&, Type=INCREMENT_VALUE_LINK);

	IncrementValueLink(const IncrementValueLink&) = delete;
	IncrementValueLink& operator=(const IncrementValueLink&) = delete;

	// Return a pointer to the value that was set.
	virtual ValuePtr execute(AtomSpace*, bool);

	static Handle factory(const Handle&);
};

LINK_PTR_DECL(IncrementValueLink)
#define createIncrementValueLink CREATE_DECL(IncrementValueLink)

/** @}*/
}

#endif // _OPENCOG_INCREMENT_VALUE_LINK_H
