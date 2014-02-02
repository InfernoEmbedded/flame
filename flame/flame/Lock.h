/*
 * FLAME_Lock.h
 *
 */

#ifndef FLAME_LOCK_H_
#define FLAME_LOCK_H_
#include <util/atomic.h>

namespace flame{

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
#endif /* FLAME_LOCK_H_ */
