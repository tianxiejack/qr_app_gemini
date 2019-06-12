#if 0
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"Zodiac_Message.h"
#include"ProcessIPCMsg.h"
#include"GLRender.h"
#include"StlGlDefines.h"
extern IPC_msg  g_MSG[2];
extern Render render;
extern ForeSightPos foresightPos;
Process_Zodiac_Message::Process_Zodiac_Message(): PanoHeight(0),
PanoLen(0),istochangeTelMode(false),telbreak(TEL_NOT_BREAK)
{

}
Process_Zodiac_Message::~Process_Zodiac_Message()
{

}
unsigned char Process_Zodiac_Message::GetdispalyMode()
 {
	 return  ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode;
 }


void Process_Zodiac_Message::save0pos()
{
	float pano_pos2angle=0.0;
	float math_ruler_angle=0.0;
	float mydelta=0.0;
	float ori_ruler_angle=0.0;
	float now_ruler_angle=0.0;
	if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.save==1
			&&ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_WHOLE_MODE)
		{
			pano_pos2angle=render.GetpWholeFacade()->GetForeSightPosX()/PanoLen*360.0;
			math_ruler_angle=render.getRulerAngle()->GetAngle();
			math_ruler_angle=math_ruler_angle+pano_pos2angle;
			if(math_ruler_angle>0.0 &&math_ruler_angle<360.0)
			{
				render.getRulerAngle()->SetAngle(math_ruler_angle);//setrulerangle(math_ruler_angle);
			}
			else if(math_ruler_angle<0.0)
			{
				render.getRulerAngle()->SetAngle(math_ruler_angle+360.0);//setrulerangle(math_ruler_angle+360.0);
			}
			else if(math_ruler_angle>360.0)
			{
				render.getRulerAngle()->SetAngle(math_ruler_angle-360.0);//setrulerangle(math_ruler_angle-360.0);
			}
			ori_ruler_angle=render.getRulerAngle()->Load();
			mydelta=(render.getRulerAngle()->GetAngle())/360.0*(render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x());
			now_ruler_angle=render.getRulerAngle()->GetAngle();
			render.getRulerAngle()->Save(now_ruler_angle);
			render.setenablerefrushruler(true);
			g_MSG[1].payload.ipc_settings.save=0;
		}
		else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.save==0)
		{

		}
}
self_check_state Process_Zodiac_Message::CheckFine()
{
	if(ReadMessage(IPC_MSG_TYPE_SELF_CHECK).payload.ipc_s_faultcode.fault_code_number==0)
	{
		return SELFCHECK_IDLE;
	}
	else if(ReadMessage(IPC_MSG_TYPE_SELF_CHECK).payload.ipc_s_faultcode.fault_code_number==1
			||ReadMessage(IPC_MSG_TYPE_SELF_CHECK).payload.ipc_s_faultcode.start_selfcheak[0]==0)
	{
		return SELFCHECK_PASS;
	}
	else
		return SELFCHECK_FAIL;
}

 void Process_Zodiac_Message::setYspeedandMove()
 {
		if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_ver
					<0)
		{
			switch(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_ver)
			{
				case -1:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0);
						break;
				}
				case -2:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*2.0);
					break;
				}
				case -3:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*5.0);
						break;
				}
				case -4:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*10.0);
						break;
				}
				case -5:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*20.0);
						break;
				}
			}
			if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_WHOLE_MODE)
				render.GetpWholeFacade()->MoveDown(-PanoHeight/5.7);
			else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_TELESCOPE_MODE)
					render.GetpTelFacade()->MoveDown(-PanoHeight/5.7);
			else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_VGA_WHITE_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_WHITE_SMALL_USELESS_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_BIG_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_SMALL_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_WOOD_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_GRASS_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_SNOW_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_DESERT_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_CITY_MODE)
			{
				render.GetpTrackFacade()->TrackMoveDown(-PanoHeight/6.0000*11.600);
			}
			else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_BIG_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_SMALL_MODE
							||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_BIG_MODE
									||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_SMALL_MODE)
			{
				render.GetpTrackFacade()->TrackMoveDown(-PanoHeight/6.0000*11.600);
			}
			else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_BIG_MODE
					||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_SMALL_MODE
							||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_BIG_MODE
									||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_SMALL_MODE)
			{
				render.GetpTrackFacade()->TrackMoveDown(-PanoHeight/6.0000*11.600);
			}
		}


		else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_ver
				>0)
		{
				switch(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_ver)
			{
				case 1:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0);
						break;
				}
				case 2:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*2.0);
					break;
				}
				case 3:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*5.0);
					break;
				}
				case 4:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*10.0);
					break;
				}
				case 5:
				{
					foresightPos.SetSpeedY((render.get_PanelLoader().Getextent_pos_z()-render.get_PanelLoader().Getextent_neg_z())/450.0*20.0);
					break;
				}
			}
				if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_WHOLE_MODE)
					render.GetpWholeFacade()->MoveUp(PanoHeight/5.7);
				else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_TELESCOPE_MODE)
					render.GetpTelFacade()->MoveUp(PanoHeight/5.7);
				else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_VGA_WHITE_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_WHITE_SMALL_USELESS_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_BIG_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_SMALL_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_WOOD_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_GRASS_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_SNOW_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_DESERT_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_CITY_MODE)
				{
					render.GetpTrackFacade()->TrackMoveUp(PanoHeight/6.0000*11.600);
				}
				else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_BIG_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_SMALL_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_BIG_MODE
												||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_SMALL_MODE)
						{
							render.GetpTrackFacade()->TrackMoveUp(PanoHeight/6.0000*11.600);
						}
						else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_BIG_MODE
								||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_SMALL_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_BIG_MODE
												||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_SMALL_MODE)
						{
							render.GetpTrackFacade()->TrackMoveUp(PanoHeight/6.0000*11.600);
						}
		}
		else
			{
				foresightPos.SetSpeedY(0);
			}

 }

	  void Process_Zodiac_Message::setXspeedandMove()
	  {
		  	  telbreak=TEL_NOT_BREAK;
			if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_hor
					<0)
			{
					switch(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_hor)
				{
					case -1:
						foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0);
						break;
					case -2:
						foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*5.0);
						break;
					case -3:
						foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*10.0);
							break;
					case -4:
						foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*20.0);
						break;
					case -5:
						foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*50.0);
							break;
				}
				if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_WHOLE_MODE)
				{
					render.GetpWholeFacade()->MoveLeft(-PanoLen*100.0);
				}
				else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_TELESCOPE_MODE)
				{
					istochangeTelMode=render.GetpTelFacade()->MoveLeft(-PanoLen/TELXLIMIT);
					if(istochangeTelMode)
					{
						telbreak=TEL_LEFT_BREAK;
					}
				}

				else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_VGA_WHITE_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_WHITE_SMALL_USELESS_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_BIG_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_SMALL_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_WOOD_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_GRASS_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_SNOW_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_DESERT_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_CITY_MODE)
						{
							render.GetpTrackFacade()->TrackMoveLeft(-PanoLen/37.685200*15.505);
						}
						else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_BIG_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_SMALL_MODE
												||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_BIG_MODE
														||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_SMALL_MODE)
								{
									render.GetpTrackFacade()->TrackMoveLeft(-PanoLen/37.685200*15.505);
								}
								else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_BIG_MODE
										||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_SMALL_MODE
												||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_BIG_MODE
														||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_SMALL_MODE)
								{
									render.GetpTrackFacade()->TrackMoveLeft(-PanoLen/37.685200*14.524);
								}


			}

			else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_hor
						>0)
				{
						switch(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.mobile_hor)
					{
						case 1:
							foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0);
							break;
						case 2:
							foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*5.0);
							break;
						case 3:
							foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*10.0);
					break;
						case 4:
							foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*20.0);
						break;
						case 5:
							foresightPos.SetSpeedX((render.get_PanelLoader().Getextent_pos_x()-render.get_PanelLoader().Getextent_neg_x())/1920.0*50.0);
								break;
					}
					if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_WHOLE_MODE)
					{
						render.GetpWholeFacade()->MoveRight(PanoLen*100.0);
					}
					else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_TELESCOPE_MODE)
					{
						istochangeTelMode=render.GetpTelFacade()->MoveRight(PanoLen/TELXLIMIT);
						if(istochangeTelMode)
						{
							telbreak=TEL_RIGHT_BREAK;
						}
					}

					else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==	RECV_VGA_WHITE_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_WHITE_SMALL_USELESS_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_BIG_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_HOT_SMALL_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_WOOD_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_GRASS_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_SNOW_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_DESERT_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_VGA_FUSE_CITY_MODE)
									{
										render.GetpTrackFacade()->TrackMoveRight(PanoLen/37.685200*15.505);
									}
									else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_BIG_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI1_WHITE_SMALL_MODE
															||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_BIG_MODE
																	||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_SDI2_HOT_SMALL_MODE)
											{
												render.GetpTrackFacade()->TrackMoveRight(PanoLen/37.685200*15.505);
											}
											else if(ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_BIG_MODE
													||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL1_WHITE_SMALL_MODE
															||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_BIG_MODE
																	||ReadMessage(IPC_MSG_TYPE_40MS_HEARTBEAT).payload.ipc_settings.display_mode==RECV_PAL2_HOT_SMALL_MODE)
											{
												render.GetpTrackFacade()->TrackMoveRight(PanoLen/37.685200*14.524);
											}
				}
			else
			{
				foresightPos.SetSpeedX(0);
			}
	  }

#endif

