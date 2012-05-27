/*
 * MHV_Lock.h
 *
 */

#ifndef MHV_LOCK_H_
#define MHV_LOCK_H_
#include <util/atomic.h>

namespace mhvlib_bsd {

class Lock {
private:
	volatile bool	_lock;

public:
	Lock();
	bool obtain();
	void release();
	bool check();
};

}
#endif /* MHV_LOCK_H_ */
