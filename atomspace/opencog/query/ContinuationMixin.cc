/*
 * ContinuationMixin.cc
 *
 * Copyright (C) 2021 Linas Vepstas
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

#include <opencog/util/exceptions.h>
#include <opencog/util/Logger.h>

#include <opencog/atoms/core/Replacement.h>
#include <opencog/atoms/execution/EvaluationLink.h>

#include "ContinuationMixin.h"

using namespace opencog;

// #define QDEBUG 1
#ifdef QDEBUG
#define DO_LOG(STUFF) STUFF
#else
#define DO_LOG(STUFF)
#endif


/**
 * Exception thrown for continuations
 */
class ContinuationException : public SilentException
{
public:
    ContinuationException(void) {}
};

class RewindException : public SilentException
{
public:
    RewindException(void) {}
};

/* ======================================================== */

/**
 */
bool ContinuationMixin::evaluate_sentence(const Handle& top,
                                          const GroundingMap& gnds)
{
	if (CONTINUATION_LINK == top->get_type())
	{
		_continuation = Replacement::replace_nocheck(top, gnds);
		throw ContinuationException();
	}
	return TermMatchMixin::evaluate_sentence(top, gnds);
}

// Assorted thread-local state. This is used to determine if we
// are entering the query engine for the first time ever, or if
// we are recursing.
static thread_local bool in_continuation = false;
static thread_local PatternLinkPtr localpat = nullptr;

bool ContinuationMixin::satisfy(const PatternLinkPtr& form)
{
	// If we are in_continuation, then this is not the first time we
	// were called. Record the pattern to be grounded, and throw an
	// exception to get us back to the first, base call.
	if (in_continuation)
	{
		localpat = form;
		throw RewindException();
	}

	PatternLinkPtr lform = form;
	int cnt = 0;

	// The exception thrown immediately above will be caught near the
	// very end of this while-loop. The loop just brings us back to the
	// beginning, with lform set to localpat.
	while (true)
	{
		// Wrap the actual satisfier in a try-catch loop. If the pattern
		// has a ContinuationLink in it, and that link is hit, then the
		// evaluate_sentence() above will throw. It records the grounding
		// that it has found. Here, we catch, take that grounding, and
		// evaluate it.
		try
		{
			return SatisfyMixin::satisfy(lform);
		}
		catch (const ContinuationException& ex) {}

		// Temporary safety mechanism. I suspect that most users do not
		// intend to write infinite loops (e.g. REPL loops) so assume
		// infinite recursion is a user error.
		cnt++;
		if (200 < cnt)
			throw InvalidParamException(TRACE_INFO,
				"Continuation looped %d times.\n"
				"Suspect an infinite continuation loop! Are you sure?\n%s\n",
				cnt, lform->to_short_string().c_str());

		DO_LOG({LAZY_LOG_FINE << "Continue cnt=" << cnt
			<< " evaluate ContinuationLink:\n"
			<< _continuation->to_short_string(); })

		// OK. If we are here, then a ContinuationLink was seen earlier in
		// the evaluation. Convert it into an ordinary PutLink, and evaluate
		// it. If, during evaluation, someone calls this method that we are
		// in right now, i.e. if they call ContinuationMixin::satisfy(), then
		// up above, we record the PatterLink to be grounded, (recording it
		// in localpat), and up above we throw. That throw is caught below,
		// thus popping the C++ stack back to here. We can now do that
		// pattern, simply by using the goto to get back to the start.
		// So .. no change in stack frame depth, no leakage of malloc'ed
		// data. We're very very good.
		try
		{
			in_continuation = true;
			Handle plk = _continuation->getOutgoingAtom(0);
			AtomSpace* tas = TermMatchMixin::_temp_aspace;
			tas->clear();
			bool crispy = EvaluationLink::crisp_eval_scratch(tas, plk, tas);

			DO_LOG({LAZY_LOG_FINE << "Finish continuing, cnt=" << cnt
				<< " result=" << crispy; })

			cnt = 0;
			in_continuation = false;
			if (crispy)
			{
				GroundingMap empty;
				propose_grounding(empty, empty);
				return search_finished(false);
			}
			return false;
		}
		catch (const RewindException& ex) {}

		DO_LOG({LAZY_LOG_FINE
			<< "***************************************";})
		DO_LOG({LAZY_LOG_FINE
			<< "Enter ContinuationMixin::satisfy cnt=" << cnt;})

		// If we are here, then the exception was caught. Make note of the
		// pattern we're supposed to ground, jump up to the top, and ground
		// it.
		in_continuation = false;
		lform = localpat;
	}

	// Not reached, obviously.
	return true;
}

/* ===================== END OF FILE ===================== */
