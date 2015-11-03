#ifndef POST_OFFICE_H_
#define POST_OFFICE_H_

#include "maraton.h"
#include "message.h"

class PostOffice:public Singleton<PostOffice>
{
public:
	int SendMail( Message* msg );
	int StartHeartBeating();
	uv_timer_t heart_beat_timer;
private:
		friend Singleton<PostOffice>;
};
#endif // !POST_OFFICE_H_
