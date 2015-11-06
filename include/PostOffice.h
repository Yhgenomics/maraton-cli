#ifndef POST_OFFICE_H_
#define POST_OFFICE_H_

#include "maraton.h"
#include "message.h"
#include "UVSockService.h"
#include <string>
using namespace std;

class PostOffice:public Singleton<PostOffice>
{
public:
	int		SendMail( Message* msg );
	int		SendSelfStatus();
	int		Init();
	int		StartHeartBeating();
	int		StartReconnectKeeper();
	int		ReconnectService();
	void	Run();

	uv_timer_t		heart_beat_timer;
	UVSockService	sock_service;
	uv_timer_t		reconnect_timer;
	int				master_port;
	string			master_ip;
	string			self_status;

private:
	friend			Singleton<PostOffice>;
};
#endif // !POST_OFFICE_H_
