#ifndef POST_OFFICE_H_
#define POST_OFFICE_H_

#include "maraton.h"
#include "Message.h"
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
	
	enum ExcutorSates
	{
		kUnknoww			=  0 ,
		kBooting			=  1 ,
		kSelfTesting		=  2 ,
		kStandby			=  3 ,
		kError				=  4 ,
		kResourceDowloading = 10 ,
		kTaskDataPreparing	= 11 ,
		kComputing			= 12 ,
		kUploading			= 13 ,
		kTaskFinished		= 14 ,
		kException			= 20
	};
	ExcutorSates    self_status;

private:
	friend			Singleton<PostOffice>;
};
#endif // !POST_OFFICE_H_
