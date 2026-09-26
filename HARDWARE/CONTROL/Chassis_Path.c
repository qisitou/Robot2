#include "headfile.h"

Chassis_PathPoint Chassis_PathPoint_StartToTurntable[2];Chassis_Path Chassis_Path_StartToTurntable;	//出发到大转盘
Chassis_PathPoint Chassis_PathPoint_StartToStairs[4];Chassis_Path Chassis_Path_StartToStairs;	    //出发到楼梯
Chassis_PathPoint Chassis_PathPoint_StartToWarehouse[4];Chassis_Path Chassis_Path_StartToWarehouse;	    //出发到仓库
Chassis_PathPoint Chassis_PathPoint_StartToHome[2];Chassis_Path Chassis_Path_StartToHome;	    //回家


/*
 *函数简介:底盘路径初始化
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Chassis_PathInit(void)
{
	/*===============出发到大转盘===============*/

	Chassis_PathPoint_StartToTurntable[0].vx=-120;
	Chassis_PathPoint_StartToTurntable[0].vy=100;
	Chassis_PathPoint_StartToTurntable[0].Delta_Angle=Straight;
	Chassis_PathPoint_StartToTurntable[0].t=200;
	Chassis_PathPoint_StartToTurntable[0].K=120;


	Chassis_PathPoint_StartToTurntable[1].vx=0;
	Chassis_PathPoint_StartToTurntable[1].vy=500;
	Chassis_PathPoint_StartToTurntable[1].Delta_Angle=Straight;
	Chassis_PathPoint_StartToTurntable[1].t=135;
	Chassis_PathPoint_StartToTurntable[1].K=600;

	Chassis_Path_StartToTurntable.End_K=600;
	Chassis_Path_StartToTurntable.Path_Size=2;
	Chassis_Path_StartToTurntable.Path=Chassis_PathPoint_StartToTurntable;

	/*===============出发到楼梯===============*/

	Chassis_PathPoint_StartToStairs[0].vx=150;
	Chassis_PathPoint_StartToStairs[0].vy=150;
	Chassis_PathPoint_StartToStairs[0].Delta_Angle=Straight;
	Chassis_PathPoint_StartToStairs[0].t=150;
	Chassis_PathPoint_StartToStairs[0].K=120;

	Chassis_PathPoint_StartToStairs[1].vx=80;
	Chassis_PathPoint_StartToStairs[1].vy=80;
	Chassis_PathPoint_StartToStairs[1].Delta_Angle=TurnRight;
	Chassis_PathPoint_StartToStairs[1].t=100;
	Chassis_PathPoint_StartToStairs[1].K=200;
	
	Chassis_PathPoint_StartToStairs[2].vx=-200;
	Chassis_PathPoint_StartToStairs[2].vy=150;
	Chassis_PathPoint_StartToStairs[2].Delta_Angle=Straight;
	Chassis_PathPoint_StartToStairs[2].t=200;
	Chassis_PathPoint_StartToStairs[2].K=200;

	Chassis_PathPoint_StartToStairs[3].vx=0;
	Chassis_PathPoint_StartToStairs[3].vy=100;
	Chassis_PathPoint_StartToStairs[3].Delta_Angle=Straight;
	Chassis_PathPoint_StartToStairs[3].t=800;
	Chassis_PathPoint_StartToStairs[3].K=120;

																	
	Chassis_Path_StartToStairs.End_K=300;
	Chassis_Path_StartToStairs.Path_Size=4;
	Chassis_Path_StartToStairs.Path=Chassis_PathPoint_StartToStairs;

	/*===============出发到仓库===============*/

	Chassis_PathPoint_StartToWarehouse[0].vx=100;
	Chassis_PathPoint_StartToWarehouse[0].vy=100;
	Chassis_PathPoint_StartToWarehouse[0].Delta_Angle=Straight;
	Chassis_PathPoint_StartToWarehouse[0].t=100;
	Chassis_PathPoint_StartToWarehouse[0].K=120;

	Chassis_PathPoint_StartToWarehouse[1].vx=80;
	Chassis_PathPoint_StartToWarehouse[1].vy=0;
	Chassis_PathPoint_StartToWarehouse[1].Delta_Angle=TurnLeft;
	Chassis_PathPoint_StartToWarehouse[1].t=100;
	Chassis_PathPoint_StartToWarehouse[1].K=120;

	Chassis_PathPoint_StartToWarehouse[2].vx=0;
	Chassis_PathPoint_StartToWarehouse[2].vy=-150;
	Chassis_PathPoint_StartToWarehouse[2].Delta_Angle=Straight;
	Chassis_PathPoint_StartToWarehouse[2].t=500;
	Chassis_PathPoint_StartToWarehouse[2].K=120;

	Chassis_PathPoint_StartToWarehouse[3].vx=-100;
	Chassis_PathPoint_StartToWarehouse[3].vy=0;
	Chassis_PathPoint_StartToWarehouse[3].Delta_Angle=Straight;
	Chassis_PathPoint_StartToWarehouse[3].t=800;
	Chassis_PathPoint_StartToWarehouse[3].K=120;



	Chassis_Path_StartToWarehouse.End_K=150;
	Chassis_Path_StartToWarehouse.Path_Size=4;
	Chassis_Path_StartToWarehouse.Path=Chassis_PathPoint_StartToWarehouse;

	/*===============回家===============*/
	Chassis_PathPoint_StartToHome[0].vx=300;
	Chassis_PathPoint_StartToHome[0].vy=0;
	Chassis_PathPoint_StartToHome[0].Delta_Angle=Straight;
	Chassis_PathPoint_StartToHome[0].t=400;   // 向右 2400mm
	Chassis_PathPoint_StartToHome[0].K=400;

	Chassis_PathPoint_StartToHome[1].vx=0;
	Chassis_PathPoint_StartToHome[1].vy=150;
	Chassis_PathPoint_StartToHome[1].Delta_Angle=Straight;
	Chassis_PathPoint_StartToHome[1].t=0;    // 向前 600mm
	Chassis_PathPoint_StartToHome[1].K=700;

	Chassis_Path_StartToHome.End_K=300;
	Chassis_Path_StartToHome.Path_Size=2;
	Chassis_Path_StartToHome.Path=Chassis_PathPoint_StartToHome;

}
