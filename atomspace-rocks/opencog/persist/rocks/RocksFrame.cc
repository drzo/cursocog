/*
 * RocksFrame.cc
 * Delete and collapse frames.
 *
 * Copyright (c) 2022 Linas Vepstas <linas@linas.org>
 *
 * LICENSE:
 * SPDX-License-Identifier: AGPL-3.0-or-later
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

#include <iomanip> // for std::quote

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/persist/sexpr/Sexpr.h>

#include "RocksStorage.h"
#include "RocksUtils.h"

using namespace opencog;

// ======================================================================

/// Delete all keys on all atoms in the indicated frame, and
/// then delete the record of the frame itself. This will leak
/// atoms, if the frame contains Atoms that do not appear in any
/// other frame. These will remain behind in the DB, orphaned.
/// These can be easily found, by searching for sids that have
/// no k@ on them.  A DB scrub routine (not implemented) could
/// "easily" remove them.
void RocksStorage::deleteFrame(AtomSpace* frame)
{
	CHECK_OPEN;
	if (not _multi_space)
		throw IOException(TRACE_INFO, "There are no frames!");

	std::string db_version = get_version();
	if (0 != db_version.compare("2"))
		throw IOException(TRACE_INFO, "DB too old to support frame deletion!");

	Handle hasp = HandleCast(frame);

	// Everything under here proceeds with the frame lock held.
	std::lock_guard<std::mutex> flck(_mtx_frame);

	// Silent return if we don't know if this AtomSpace.
	// Presumably, it was deleted earlier, or never stored.
	const auto& pr = _frame_map.find(hasp);
	if (_frame_map.end() == pr)
		return;

	// I'm too lazy to implement delete-from-the-middle. So throw
	// if this is a non-top frame.
	for (const Handle& hi : hasp->getIncomingSet())
		if (_frame_map.end() != _frame_map.find(hi))
			throw IOException(TRACE_INFO,
				"Deletion of non-top frames is not currently supported!\n");

	// OK, we've got the frame to delete.
	// First, get rid of all the atoms in it.
	std::string fid = pr->second + ":";
	std::string oid = "o@" + fid;

	// Loop over all atoms in the frame, and delete any keys on them.
	size_t sidoff = oid.size();
	auto it = _rfile->NewIterator(rocksdb::ReadOptions());
	for (it->Seek(oid); it->Valid() and it->key().starts_with(oid); it->Next())
	{
		const std::string& fis = it->key().ToString();
		const std::string& sid = fis.substr(sidoff);

		// Delete all values hanging on the atom ...
		std::string pfx = "k@" + sid + ":" + fid;
		auto kt = _rfile->NewIterator(rocksdb::ReadOptions());
		for (kt->Seek(pfx); kt->Valid() and kt->key().starts_with(pfx); kt->Next())
			_rfile->Delete(rocksdb::WriteOptions(), kt->key());
		delete kt;

		// Delete the key itself
		_rfile->Delete(rocksdb::WriteOptions(), it->key());
	}
	delete it;

	// Delete the frame encoding, too.
	fid = pr->second;
	std::string did = "d@" + fid;
	std::string senc;
	_rfile->Get(rocksdb::ReadOptions(), did, &senc);
	_rfile->Delete(rocksdb::WriteOptions(), did);
	_rfile->Delete(rocksdb::WriteOptions(), "f@" + senc);

	// Finally, remove it from out own tables.
	_fid_map.erase(fid);
	_frame_map.erase(hasp);
}

// ======================================================================

// If the existing open database is not in multi-space format, then
// convert it to the multi-space format. This requires looping over
// all keys in the database, and changing their format: the key
// indexes must now include an ID of which AtomSpace they belong to.
void RocksStorage::convertForFrames(const Handle& top)
{
	if (_multi_space) return;
	_multi_space = true;

	writeFrame(top);

	// Do we need to perform a conversion?
	std::string pfx = "a@";
	auto it = _rfile->NewIterator(rocksdb::ReadOptions());
	it->Seek(pfx);
	it->Next(); // skip over (PredicateNode "*-TruthValueKey-*")
	if (not (it->Valid() and it->key().starts_with(pfx)))
	{
		delete it;
		return;
	}
	delete it;

	// Find the bottom-most frame, and assume that is the intended base.
	Handle bot = top;
	size_t osz = bot->get_arity();
	while (0 < osz)
	{
		if (1 < osz)
			throw IOException(TRACE_INFO, "Non-unique bottom frame!");
		bot = bot->getOutgoingAtom(0);
		osz = bot->get_arity();
	}

	// Get the frame ID to which everything will be consigned to.
	std::string fid = writeFrame(bot) + ":";

	// Loop over all atoms, and convert keys.
	it = _rfile->NewIterator(rocksdb::ReadOptions());
	for (it->Seek(pfx); it->Valid() and it->key().starts_with(pfx); it->Next())
	{
		std::string akey = it->key().ToString();
		const std::string& sid = akey.substr(2, akey.length()-3);

		size_t nkeys = 0;
		akey[0] = 'k';
		auto kit = _rfile->NewIterator(rocksdb::ReadOptions());
		for (kit->Seek(akey);
			kit->Valid() and kit->key().starts_with(akey); kit->Next())
		{
			std::string kid = kit->key().ToString();
			std::string skid = kid;
			skid.insert(skid.find(':') + 1, fid);

			const std::string& kval = kit->value().ToString();
			_rfile->Put(rocksdb::WriteOptions(), skid, kval);

			_rfile->Delete(rocksdb::WriteOptions(), kid);
			nkeys ++;
		}
		delete kit;

		// If there were no keys, write the marker.
		if (0 == nkeys)
			_rfile->Put(rocksdb::WriteOptions(), akey + fid + "+1", "");

		// Write the frame membership.
		_rfile->Put(rocksdb::WriteOptions(), "o@" + fid + sid, "");

		// Compute the height, and store that.
		Handle h = Sexpr::decode_atom(it->value().ToString());
		size_t height = getHeight(h);
		if (0 < height)
			_rfile->Put(rocksdb::WriteOptions(),
				"z" + aidtostr(height) + "@" + sid, "");
	}
	delete it;
}

// ======================================================================

/// Perform some consistency checks
bool RocksStorage::checkFrames(void)
{
	CHECK_OPEN;
	if (not _multi_space) return true;

	// Look for atoms that have no keys on them.
	std::string pfx = "a@";
	size_t cnt = 0;
	auto it = _rfile->NewIterator(rocksdb::ReadOptions());
	for (it->Seek(pfx); it->Valid() and it->key().starts_with(pfx); it->Next())
	{
		std::string akey = it->key().ToString();
		akey[0] = 'k';
		auto kt = _rfile->NewIterator(rocksdb::ReadOptions());
		kt->Seek(akey);
		if (not (kt->Valid() and kt->key().starts_with(akey)))
		{
			// `a@1:` is the key for (PredicateNode "*-TruthValueKey-*")
			// and ignore that as a special case.
			if (akey.compare("k@1:"))
				cnt++;
		}
		delete kt;
	}
	delete it;

	if (cnt)
	{
		printf("Info: found %zu orphaned Atoms!\n", cnt);
		printf("These can be removed by running `cog-rocks-scrub`\n");
		return false;
	}
	return true;
}

// ======================================================================

/// Scrub away any orphaned Atoms resulting from frame deletion.
void RocksStorage::scrubFrames(void)
{
	CHECK_OPEN;
	if (not _multi_space) return;

	size_t cnt = 0;

	std::string pfx = "a@";
	auto it = _rfile->NewIterator(rocksdb::ReadOptions());
	for (it->Seek(pfx); it->Valid() and it->key().starts_with(pfx); it->Next())
	{
		std::string akey = it->key().ToString();
		akey[0] = 'k';
		auto kt = _rfile->NewIterator(rocksdb::ReadOptions());
		kt->Seek(akey);
		if (kt->Valid() and kt->key().starts_with(akey))
		  { delete kt; continue; }

		delete kt;

		// `a@1:` is the key for (PredicateNode "*-TruthValueKey-*")
		// Ignore it as a special case.
		if (0 == akey.compare("k@1:")) continue;

		// We've found an orphan. Delete the `a@` index entry.
		std::string satom = it->value().ToString();
		akey[0] = 'a';
		_rfile->Delete(rocksdb::WriteOptions(), akey);

		// Delete the incoming sets, too.
		// To get fancy, could use DeleteRange() here.
		akey[0] = 'i';
		auto ic = _rfile->NewIterator(rocksdb::ReadOptions());
		for (ic->Seek(akey); ic->Valid() and ic->key().starts_with(akey); ic->Next())
		{
			std::string inky = ic->key().ToString();
			_rfile->Delete(rocksdb::WriteOptions(), inky);
		}

		// We won't know if it is a Node or Link till we decode it.
		Handle orph =  Sexpr::decode_atom(satom);
		if (orph->is_node())
			_rfile->Delete(rocksdb::WriteOptions(), "n@" + satom);
		else
		{
			_rfile->Delete(rocksdb::WriteOptions(), "l@" + satom);

			// Also delete the zN@sid entries.
			size_t height = getHeight(orph);
			const std::string& sid = akey.substr(2);
			_rfile->Delete(rocksdb::WriteOptions(),
				"z" + aidtostr(height) + "@" + sid);
		}

		cnt++;
	}

	printf("Deleted %zu orphaned Atoms.\n", cnt);
}

// ======================== THE END ======================
