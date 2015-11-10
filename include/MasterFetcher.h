#ifndef MASTER_FETCHER_H_
#define MASTER_FETCHER_H_

#include "MasterSession.h"

class MasterFetcher : public Singleton<MasterFetcher>
{
public:
	MasterSession*	master_pointer()						{ return master_pointer_;	}
	void			master_pointer( MasterSession* value )	{ master_pointer_ = value;	}

private:
	MasterSession*	master_pointer_;
	friend			Singleton<MasterFetcher>;
};

#endif // !MASTER_FETCHER_H_