#ifndef _TASK_SCREEN_H
#define _TASK_SCREEN_H

#include "task_app.hpp"
#include "lcd.hpp"

#include "stringstream.hpp"
using LCD::lcd;
using lib::StringStream;
extern lcd<12> screen;
extern StringStream<20> sstream;
void task_screen_flush_Init();

#endif