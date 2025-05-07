/*
 * SexprColumn.cc
 *
 * Copyright (C) 2015, 2022, 2025 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  January 2009
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the
 * exceptions at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <opencog/atoms/value/LinkValue.h>
#include <opencog/atoms/value/StringValue.h>

#include "SexprColumn.h"

using namespace opencog;

SexprColumn::SexprColumn(const HandleSeq&& oset, Type t)
	: Link(std::move(oset), t)
{
	if (not nameserver().isA(t, SEXPR_COLUMN))
	{
		const std::string& tname = nameserver().getTypeName(t);
		throw InvalidParamException(TRACE_INFO,
			"Expecting a SexprColumn, got %s", tname.c_str());
	}

	size_t sz = _outgoing.size();

	if (1 != sz)
		throw InvalidParamException(TRACE_INFO,
			"SexprColumn expects one arg, got %lu", sz);
}

// ---------------------------------------------------------------

/// Return a StringValue vector.
ValuePtr SexprColumn::do_execute(AtomSpace* as, bool silent)
{
	// If the given Atom is executable, then execute it.
	Handle base(_outgoing[0]);
	if (base->is_executable())
	{
		ValuePtr vp(base->execute(as, silent));
		if (vp->is_atom())
			base = HandleCast(vp);
		else
		{
			if (not vp->is_type(LINK_VALUE))
				return createStringValue(vp->to_string());

			// If we are here, we've got a LinkValue
			std::vector<std::string> svec;
			svec.reserve(vp->size());
			for (const ValuePtr& v : LinkValueCast(vp)->value())
				svec.push_back(v->to_short_string());
			return createStringValue(std::move(svec));
		}
	}

	// If we are here, then base is an atom.
	if (base->is_node())
		return createStringValue(base->to_short_string());

	// If we are here, then base is an link.
	std::vector<std::string> svec;
	svec.reserve(base->get_arity());
	for (const Handle& h : base->getOutgoingSet())
		svec.push_back(h->to_short_string());

	return createStringValue(std::move(svec));
}

// ---------------------------------------------------------------

/// Return a StringValue vector.
ValuePtr SexprColumn::execute(AtomSpace* as, bool silent)
{
	return do_execute(as, silent);
}

DEFINE_LINK_FACTORY(SexprColumn, SEXPR_COLUMN)

/* ===================== END OF FILE ===================== */
