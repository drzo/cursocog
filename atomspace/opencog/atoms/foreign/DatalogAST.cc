/*
 * DatalogAST.cc
 *
 * Copyright (C) 2021 Linas Vepstas
 *
 * Author: Linas Vepstas <linasvepstas@gmail.com>  October 2021
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

#include <opencog/persist/prolog/Prolog.h>

#include "DatalogAST.h"

using namespace opencog;

void DatalogAST::init()
{
	if (not nameserver().isA(_type, DATALOG_AST))
	{
		const std::string& tname = nameserver().getTypeName(_type);
		throw InvalidParamException(TRACE_INFO,
			"Expecting an DatalogAST, got %s", tname.c_str());
	}
}

DatalogAST::DatalogAST(const HandleSeq&& oset, Type t)
	: ForeignAST(std::move(oset), t)
{
	init();
}

DatalogAST::DatalogAST(const HandleSeq&& oset, const std::string&& sexpr)
	: ForeignAST(std::move(oset), DATALOG_AST)
{
	init();
	_name = sexpr;
}

DatalogAST::DatalogAST(const std::string& sexpr)
	: ForeignAST(DATALOG_AST)
{
	parse(sexpr);
}

// ---------------------------------------------------------------

/// Parse one or more clauses, e.g.
/// foo(X) :- bar(X). bing(bang,bong). food(pizza).
void DatalogAST::parse(const std::string& sexpr)
{
	size_t l = 0;
	size_t r = 0;
	_outgoing = Prolog::parse(sexpr, l, r);
}

// ---------------------------------------------------------------

std::string DatalogAST::to_string(const std::string& indent) const
{
	if (0 == _outgoing.size())
		return indent + "(DatalogAst \"" + _name + "\") ; " + id_to_string();

	std::string rv = indent + "(DatalogAst\n";
	for (const Handle& h: _outgoing)
		rv += h->to_string(indent + "  ") + "\n";

	rv += indent + ") ; " + id_to_string();
	return rv;
}

std::string DatalogAST::to_short_string(const std::string& indent) const
{
	if (0 == indent.size())
		return _name + "\n" + to_short_string(";") + "\n";

	// Debugging print
	if (0 == _outgoing.size()) // this should never happen
		return _name + "XXX-borken";

	std::string rv = "";
	for (const Handle& h: _outgoing)
	{
		if (DATALOG_AST == h->get_type())
			rv += h->to_short_string("xx") + " ";
		else
			rv += indent + h->to_short_string(indent);
	}

	return rv;
}

// ---------------------------------------------------------------
// Custom factory, because its a hermaphrodite. The ForgeinAST will
// pass us a string, behaving like a node, which we parse into an
// expression tree.

Handle DatalogAST::factory(const Handle& base)
{
	/* If it's castable, nothing to do. */
	if (DatalogASTCast(base)) return base;

	if (0 == base->get_arity())
		return HandleCast(createDatalogAST(std::move(base->get_name())));

	return HandleCast(createDatalogAST(
		std::move(base->getOutgoingSet()),
		std::move(Prolog::prt_datalog(base))));
}

/* This runs when the shared lib is loaded. */
static __attribute__ ((constructor)) void init_sexprast_factory(void)
{
	classserver().addFactory(DATALOG_AST, &DatalogAST::factory);
}

/* ===================== END OF FILE ===================== */
