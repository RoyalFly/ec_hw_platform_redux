//
// Created by 98383 on 2023/11/12.
//

#include "..\base\Remote\Remote.h"
#include "..\base\motor\Driver\Dji_Motor_Driver.h"
#include "..\base\motor\motor.h"
#include "..\hardware_config.h"
#include "iwdg.h"
#include <cmath>

#ifdef DR16
RC rc(RC_UART);
#endif

bool robot_init_flag = false;
const float motor_stop_rotate_speed_thres = 1200;

RC::RCswitch last_rc_switch;

enum RobotPowerState_e { //上电状态
  STOP = 0,
  STARTUP = 1,
  WORKING = 2,
} robot_state;

extern DJIMotorDriver dji_motor_driver;
extern Motor User_Motor;

void MainControlinit(){
  rc.init();
  dji_motor_driver.idConfig();
  User_Motor.reset();
}

void iwdghandler(bool iwdg_flag){
  if (iwdg_flag){
    HAL_IWDG_Refresh(&hiwdg);
  }
}

void robotPowerStateFSM(bool stop_flag){
  if (robot_state == STOP){
    if (!stop_flag){
      robot_state = STARTUP;
    }
  } else
  if (robot_state == STARTUP){
    if (stop_flag){
      robot_state = STOP;
    } else
    if (robot_init_flag){
      robot_state = WORKING;
    }
  } else
  if (robot_state == WORKING){
    if (stop_flag){
      robot_state = STOP;
    }
  }
  if (robot_state == STOP){
    robot_init_flag = false;
  }
}

void allMotorsStopShutoff(void){
  if (User_Motor.mode_ != Motor::INIT){
    if (fabs(User_Motor.motor_data_.rotate_speed) > motor_stop_rotate_speed_thres){
      User_Motor.mode_ = Motor::STOP;
    } else {
      User_Motor.mode_ = Motor::POWEROFF;
    }
  }
}

void allMotorsOn(void){
  if (User_Motor.mode_ != Motor::INIT){
    User_Motor.mode_ = Motor::WORKING;
  }
}

void robotreset(void){
  robot_init_flag = false;
  last_rc_switch = rc.switch_;
}

void Robot_Control(void){
  if (rc.switch_.l == RC::UP && rc.switch_.r == RC::MID){
    User_Motor.setSpeed(rc.channel_.r_col / 10.f);
  }
}

void MainControlloop(){
  iwdghandler(rc.connect_.check());
  rc.handle();

  robotPowerStateFSM(!rc.connect_.check() || rc.switch_.r == RC::DOWN);
  if (robot_state == STOP){
    allMotorsStopShutoff();
    robotreset();
  } else
  if (robot_state == STARTUP){
    allMotorsOn();
    robot_init_flag = true;
  } else
  if (robot_state == WORKING){
    allMotorsOn();
    Robot_Control();
  }

  User_Motor.handle();

  dji_motor_driver.canTxMsg(1, djimotor::ID_1_4);
  dji_motor_driver.canTxMsg(1, djimotor::ID_5_8);
  dji_motor_driver.canTxMsg(1, djimotor::ID_9_11);

  dji_motor_driver.canTxMsg(2, djimotor::ID_1_4);
  dji_motor_driver.canTxMsg(2, djimotor::ID_5_8);
}