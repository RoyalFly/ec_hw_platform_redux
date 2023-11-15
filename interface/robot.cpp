//
// Created by 98383 on 2023/11/12.
//

#include "..\base\Remote\Remote.h"
#include "..\hardware_config.h"
#include "iwdg.h"

#ifdef DR16
RC rc(RC_UART);
#endif

void MainControlinit(){
  rc.init();
}

void iwdghandler(bool iwdg_flag){
  if (iwdg_flag){
    HAL_IWDG_Refresh(&hiwdg);
  }
}

void MainControlloop(){
  iwdghandler(rc.connect_.check());
  rc.handle();
}