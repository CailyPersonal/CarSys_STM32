///////////////////////////// start of file ///////////////////////////////////
#include "PublicHeader.h"

void ConnectToWINCC(void);
bool TryToGetMsgFromWINCC(void);
void SendCarRunStatusToWINCC(void);
void SendCarTaskStatusToWINCC(float yaw, u8 TaskCount, u8 TaskIndex);
void SendCarYawAndSpeedToWINCC(float yaw);
void SendFinishSingleTask(u16 deviation);
void SuperviseCurrentYaw(float yaw);
void SuperviseAllTaskCount(u8 count);
void SuperviseTaskDetail(u8 index, u16 x, u16 y);
void SuperviseTaskIndexAndTargetPos(u8 index, u16 x, u16 y);
void SuperviseTargetAndPresentYaw(float TargetYaw, float CurrentYaw);
void SuperviseTaskFinish(u16 error);




