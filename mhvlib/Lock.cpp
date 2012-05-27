/*
 * Lock.cpp
 *
 *  Created on: 26/06/2011
 *      Author: deece
 */

#include <mhvlib/Lock.h>

namespace mhvlib_bsd {

/**
 * Create a new lock
 */
Lock::Lock() {
	_lock = false;
}

/**
 * Obtain the lock
 * @return true if the lock was successfully obtained, false otherwise
 */
bool Lock::obtain() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		if (!_lock) {
			_lock = true;
			return true;
		}
	}
	return false;
}

/**
 * Release the lock
 */
void Lock::release() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		_lock = false;
	}
}

/**
 * Check if the lock is currently held
 * @return true if the lock is held
 */
bool Lock::check() {
	return _lock;
}

}
