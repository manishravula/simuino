/*  Simuino is a Arduino Simulator based on Servuino Engine
    Copyright (C) 2012  Benny Saxen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. 
*/


//====================================   
int milliSleep(unsigned long milisec)
//====================================   
{
  struct timespec req={0};
  time_t sec=(int)(milisec/1000);
  milisec=milisec-(sec*1000);
  req.tv_sec=sec;
  req.tv_nsec=milisec*1000000L;
  while(nanosleep(&req,&req)==-1)
    continue;
  return 1;
}

//====================================   
int microSleep(unsigned long microsec)
//====================================   
{
  struct timespec req={0};
  time_t sec=(int)(microsec/1000000L);
  microsec=microsec-(sec*1000000L);
  req.tv_sec=sec;
  req.tv_nsec=microsec*1000;
  while(nanosleep(&req,&req)==-1)
    continue;
  return 1;
}

//====================================
void iDelay(int ms)
//====================================
{
  milliSleep(ms);
}

//====================================
void microDelay(int us)
//====================================
{
  microSleep(us);
}

//====================================
int analyzeEvent(char *event)
//====================================
{
  int step,pin,value;
  char str[80];
  {
    if(strstr(event,"analogRead") || strstr(event,"digitalRead"))
      {
	sscanf(event,"%s %d %d",str,&pin,&value);
	g_pinNo = pin;
	g_pinValue = value;
	if(strstr(event,"analog"))  g_pinType = ANA;
	if(strstr(event,"digital")) g_pinType = DIG;
	return(g_pinType);
      } 
    return(0); 
  }
}
//====================================
void show(WINDOW *win)
//====================================
{
  int next;

  box(win,0,0);
  if(win == uno) 
    {
      wmove(win,0,2);
      if(confBoardType ==UNO)
	wprintw(win,"SIMUINO - Arduino UNO Pin Analyzer 0.1.5");
      if(confBoardType ==MEGA)
	wprintw(win,"SIMUINO - Arduino MEGA Pin Analyzer 0.1.5");
      wmove(win,1,2);
      wprintw(uno,"Project......: %s              ",currentConf);
      wmove(win,2,2);
      wprintw(uno,"Loaded Sketch: %s              ",appName);
    }
  if(win == ser)
    {
      wmove(win,0,2);
      wprintw(win,"Serial Interface");
    }
  if(win == slog)
    {
      next =  loopPos[currentLoop+1];
      if(currentStep == loopPos[currentLoop+1]) next = loopPos[currentLoop+2];
      wmove(win,0,2);
      wprintw(win,"Log  ");
      if(currentStep == g_steps)
	wprintw(slog,"%d,%d ->| (%d,%d)",currentLoop,currentStep,g_loops,g_steps);
      else
	wprintw(slog,"%d,%d ->%d (%d,%d)",currentLoop,currentStep,next,g_loops,g_steps);
    }
  if(win == msg)
    {
      wmove(win,0,2);
      wprintw(win,"Messages");
    }
  
  wmove(uno,board_h-2,4);
  wprintw(uno,"                  ");
  wmove(uno,board_h-2,4);
  wrefresh(uno);
  wrefresh(win);
}

//====================================
void putMsg(int line,const char *message)
//====================================
{
  wclear(msg);
  wmove(msg,line,1);
  wprintw(msg,message);
  show(msg);
  return;
}
//====================================
void errorLog(const char msg[], int x)
//====================================
{
  showError(msg,x);
}
//====================================
void showError(const char *m, int value)
//====================================
{
  char err_msg[300];
  strcpy(err_msg,"SimuinoERROR: ");
  strcat(err_msg,m);
  fprintf(err,"%s %d\n",err_msg,value,-1);
  error = 1;
}
//====================================
void addConfList(char *cf)
//====================================
{
  return;
}
//====================================
void delConfList(char *cf)
//====================================
{ 
  return;
}
//====================================
void saveConfig(char *cf)
//====================================
{
  FILE *out;
  time_t lt;

  out = fopen(cf,"w");
  if(out == NULL)
    {
      showError("No config file ",-1);
    }
  else
    {
      putMsg(msg_h-2,"Configuration saved");
      lt = time(NULL);
      fprintf(out,"# Simuino Configuration %s",ctime(&lt));
      fprintf(out,"# %s\n",cf);

      //fprintf(out,"BOARD_TYPE %d\n",confBoardType);

      if(confSteps > MAX_STEP)confSteps = MAX_STEP; 
      fprintf(out,"SIM_LENGTH %d\n",confSteps);

      if(confWinMode >=0 && confWinMode <= WIN_MODES)
	fprintf(out,"WIN_LAYOUT %d\n",confWinMode);
      else
	confWinMode = 0;

      fprintf(out,"SKETCH     %s\n",confSketchFile);
    }
  fclose(out);

  // Store setting
  out = fopen("settings.txt","w");
  if(out == NULL)
    {
      showError("No setting file ",-1);
    }
  else
    {
      lt = time(NULL);
      fprintf(out,"# Simuino Setting %s",ctime(&lt));
      fprintf(out,"PROJECT: %s\n",cf);
    }
  fclose(out);
}

//====================================
void resetFile(const char *filename)
//====================================
{
  FILE *out;
  time_t lt;

  out = fopen(filename,"w");
  if(out == NULL)
    {
      showError("Unable to reset file ",-1);
      return;
    }
  else
    {
      lt = time(NULL);
      fprintf(out,"# Simuino Log File %s",ctime(&lt));
      fclose(out);
    }
}


//====================================
void fill(int len,char *p,char c)
//====================================
{
  int i;

  if(len > 0)
    {
      for(i=0;i<len;i++)p[i]=c;
      p[len]='\0';
    }
}
//====================================
void winLog()
//====================================
{
  int i,k;
  char filler[120];

  wmove(slog,1,1);
  fill(log_w-strlen(simulation[currentStep+1]),filler,' ');
  wprintw(slog,"next>%s%s",simulation[currentStep+1],filler);
  for(i=1;i<log_h-2;i++)
    {
      wmove(slog,i+1,1);
      k = currentStep - i+1;
      if(k>0)
	{
	  fill(log_w-strlen(simulation[k]),filler,' ');
	  wprintw(slog,"[%d,%d] %s%s",k,stepLoop[k],simulation[k],filler);
	}
      else
	wprintw(slog,"%s",logBlankRow);
    }
  show(slog);
}

//====================================
void winSer()
//====================================
{
  int i,j,k=0,m=0,prevL=1;
  char buf[MAX_STEP][240];
  char filler[120];

  for(i=1;i<=currentStep;i++)
    {
      if(strlen(serial[i]) > 0)
	{
	  if(prevL == 1) //New Line
	    {
	      m++;
	      strcpy(buf[m],serial[i]);
	    }
	  else
	    strcat(buf[m],serial[i]);

	  prevL = serialL[i];
	}
    }

  wmove(ser,1,1);
  for(i=1;i<ser_h-1;i++)
    {
      wmove(ser,i,1);
      k = m-i+1;
      if(k>0)
	{
	  fill(ser_w-strlen(buf[k]),filler,' ');
	  wprintw(ser,"%s%s",buf[k],filler);
	}
      else
	wprintw(ser,"%s",logBlankRow);
    }


  show(ser);
}

//====================================
void unoInfo()
//====================================
{

  wmove(uno,ap+2,3); 

  if(g_existError == YES)
    wprintw(uno,"  [Errors - err]");
  else if(g_warning == YES)
    wprintw(uno,"  [Possible Mismatch - load]");
  else
    wprintw(uno,"                           ");

  show(uno);
}

//====================================
void showLoops()
//====================================
{
  int i;
  wclear(msg);
  wmove(msg,1,2);
  wprintw(msg," Loop information:");
  for(i=0;i<g_loops;i++)
    {
      if(i < msg_h-1)
	{
	  wmove(msg,2+i,2);
          if(currentLoop == i)
	    wprintw(msg,">%3d: %4d -> %4d",i,loopPos[i]+1,loopPos[i+1]);
          else
            wprintw(msg," %3d: %4d -> %4d",i,loopPos[i]+1,loopPos[i+1]);
	}
    }
  show(msg);
}


//====================================
int wCustomLog(char *in, char *out)
//====================================
{
  char *q,*p;
  int pin=0;

  p = strstr(in,":");
  if(p==NULL) 
  {
    showError("Custom Log: Cant find any semicolon",0);
    strcpy(out,"Custom : Failed"); 
    return(0);
  }

  p++; 
  sscanf(p,"%d",&pin); 
  p = strstr(p,"\""); 
  if(p==NULL)
  {
    showError("Custom Log: Cant find first \" ",0);
    strcpy(out,"Custom \" Failed");
    return(0);
  }

  p++; 
  q = strstr(p,"\""); 
  if(q==NULL)
  {
    showError("Custom Log: Cant find second \" ",0);
    strcpy(out,"Custom \" Failed");
    return(0);
  }

  strcpy(q,"\0"); 
  strcpy(out,p);
  return(pin);
}

//====================================
void getString(char *in, char *out)
//====================================
{
  char *q,*p;

  p = strstr(in,"'");
  p++;
  q = strstr(p,"'");
  strcpy(q,"\0");
  strcpy(out,p);
  return;
}

//====================================
void readSketchInfo()
//====================================
{
  FILE *in;
  char row[80],res[40],*p,*q,value[5];
  int pin;

  in = fopen(fileServSketch,"r");
  if(in == NULL)
    {
      showError("No servuino/sketch.pde",-1);
    }
  else
    {
      while (fgets(row,80,in)!=NULL)
	{

	  if(p=strstr(row,"SKETCH_NAME:"))
	    {
	      q = strstr(p,":");q++;
	      sscanf(q,"%s",appName);
	    }
	  if(p=strstr(row,"BOARD_TYPE"))
	    {
	      if(strstr(row,"UNO") != NULL) confBoardType = UNO;
	      if(strstr(row,"MEGA")!= NULL) confBoardType = MEGA;
	    }
	  if(p=strstr(row,"PINMODE_IN:"))
	    {
	      pin = wCustomLog(p,res);
	      strcpy(textPinModeIn[pin],res);
	    }
	  if(p=strstr(row,"PINMODE_OUT:"))
	    {
	      pin = wCustomLog(p,res);
	      strcpy(textPinModeOut[pin],res);
	    }
	  if(p=strstr(row,"DIGITALWRITE_LOW:"))
	    {
	      pin = wCustomLog(p,res);
	      strcpy(textDigitalWriteLow[pin],res);
	    }
	  if(p=strstr(row,"DIGITALWRITE_HIGH:"))
	    {
	      pin = wCustomLog(p,res);
	      strcpy(textDigitalWriteHigh[pin],res);
	    }
	  if(p=strstr(row,"ANALOGREAD:"))
	    {
	      pin = wCustomLog(p,res);
	      strcpy(textAnalogRead[pin],res);
	    }
	  if(p=strstr(row,"DIGITALREAD:"))
	    {
	      pin = wCustomLog(p,res);
	      strcpy(textDigitalRead[pin],res);
	    }
	  if(p=strstr(row,"ANALOGWRITE:"))
	    {
	      pin = wCustomLog(p,res);
	      strcpy(textAnalogWrite[pin],res);
	    }
	}
      fclose(in); 
    }
}

//====================================
void initSim()
//====================================
{
  int i;

  strcpy(appName,"-");

  for(i=0;i<MAX_STEP;i++)
    {
      strcpy(simulation[i],"");
      strcpy(serial[i],"");
      strcpy(simComment[i],"");
      stepComment[i] = 0;
      serialL[i] = 0;
    }
  for(i=0;i<MAX_LOOP;i++)
    {
      loopPos[i] = 0;
    }
  for(i=0;i<MAX_PIN_DIGITAL_MEGA;i++)
    {
      strcpy(textPinModeIn[i],"void");
      strcpy(textPinModeOut[i],"void");

      strcpy(textDigitalWriteLow[i],"void");
      strcpy(textDigitalWriteHigh[i],"void");

      strcpy(textAnalogWrite[i],"void");
      strcpy(textDigitalRead[i],"void");
   
      currentValueD[i] = 0;
    }
  for(i=0;i<MAX_PIN_ANALOG_MEGA;i++)
    {
      strcpy(textAnalogRead[i],"void");
      currentValueA[i] = 0;
    }

}
//====================================
void resetSim()
//====================================
{
  int i;
  currentStep = 1;
  currentLoop = 0;
  for(i=0;i<max_digPin;i++)
    {
      digitalMode[i] = FREE;
    }
}

//====================================
void readConfig(char *cf)
//====================================
{
  FILE *in;
  char row[80],*p,temp[40];
  int x;
  
  in = fopen(cf,"r");
  if(in == NULL)
    {
      showError("No config file",-1);
      confSteps = 444;
      confWinMode = 2;
      strcpy(confSketchFile,"helloWorld_UNO.c");
      return;
    }
  else
    {
      while (fgets(row,80,in)!=NULL)
	{
	  if(row[0] != '#')
	    {
	      if(p=strstr(row,"SIM_LENGTH"))
		{
		  sscanf(p,"%s%d",temp,&confSteps);
		}
	      if(p=strstr(row,"WIN_LAYOUT"))
		{
		  sscanf(p,"%s%d",temp,&confWinMode);
		}
              if(p=strstr(row,"SKETCH"))
                {
                  sscanf(p,"%s%s",temp,confSketchFile);
                }
	    }
	 
	}
    }
  fclose(in);
}
//====================================
int readEvent(char *ev, int step)
//====================================
{
  if(step > 0 && step <= g_steps)
    strcpy(ev,simulation[step]);
  else
    return(0);
  return(step);
}    


//====================================
void runLoop(int dir)
//====================================
{
  int tmp;

  runStep(dir);

  if(currentLoop ==  g_loops)
    {
      while(currentStep < g_steps)
	runStep(dir);
    }

  else if(currentLoop >= 0 && currentLoop < g_loops)
    {
      if(dir == FORWARD)
	{
	  tmp = loopPos[currentLoop+1]-1;
	  while(currentStep < tmp)
	    {
	      runStep(FORWARD);
	    }
	}
      if(dir == BACKWARD)
	{
	  tmp = loopPos[currentLoop]-1;
	  while(currentStep > tmp)
	    {
	      runStep(BACKWARD);
	    }
	}
    }
  return;
}    

//====================================
void runLoops(int targetLoop)
//====================================
{
  int stop;
  targetLoop = checkRange(HEAL,"loop",targetLoop);
  while(currentLoop < targetLoop && stop == 0)
    {
      stop = runStep(FORWARD);
    }
  return;
}    

//====================================
void runAll(int stop)
//====================================
{
  int x;
  stop = checkRange(HEAL,"step",stop);

  if(currentStep < stop)
    {
      while(currentStep < stop)
	x = runStep(FORWARD);
    }

  if(currentStep > stop)
    {
      while(currentStep > stop)
	x = runStep(BACKWARD);
    }

  return;
}    

//====================================
void endOfSimulation()
//====================================
{
  wmove(slog,1,1);
  wprintw(slog,"%s",logBlankRow);
  wscrl(slog,-1);
  wmove(slog,1,1);
  wprintw(slog,">%s",logBlankRow);
  //wmove(slog,2,1);
  //wprintw(slog,"-=End of Simulation=-");
  show(slog);
  return;
}    

//====================================
void runNextRead()
//====================================
{
  char event[80];
  int x;

  while (!strstr(event,"digitalRead") && !strstr(event,"analogRead") && currentStep < g_steps)
    {
      runStep(FORWARD);
      x = readEvent(event, currentStep+1);
    }
  return;
}    


//====================================
void readSimulation()
//====================================
{
  FILE *in;
  char row[SIZE_ROW],*p,temp[SIZE_ROW],junk[5];
  int step=0,loop=0;
  
  g_steps       = 0;
  g_loops       = 0;
  g_comments    = 0;
  scenAnalog    = 0;
  scenDigital   = 0;
  scenInterrupt = 0;

  in = fopen(fileServArduino,"r");
  if(in == NULL)
    {
      showError("No simulation file",-1);
    }
  else
    {
      while (fgets(row,SIZE_ROW,in)!=NULL)
	{
	  
	  if(row[0] == '+')
	    {
	      p = strstr(row," ? ");
	      p = p+3;
	      g_steps++;
	      sscanf(row,"%s%d",junk,&step);
	      if(step == g_steps)
		strcpy(simulation[step],p);
	      else
		{
		  showError(row,step);
		  showError(row,g_steps);
		}
	      if(p=strstr(row,"servuinoLoop "))
		{
		  sscanf(p,"%s%d",temp,&loop);
		  loopPos[loop] = step;
		  loopStep[loop] = step;
		  sprintf(simulation[step],"Loop %d",loop);
		}
	      stepLoop[step] = loop;
	    }
	  if(row[0] == '=')
	    {
	      g_comments++;
	      p = strstr(row,"= ");
              p = p+2;
	      sscanf(row,"%s%d",junk,&step);
              stepComment[0] = g_comments;
              if(step == g_steps)
		{
		  stepComment[g_comments] = step;
		  commentStep[step] = g_comments;
		  strcat(simComment[g_comments],p);
		}
	    }
	  
          else if(p=strstr(row,"ENDOFSIM"))
            {
	      loop++;
              loopPos[loop]  = step+1;
	      loopStep[loop] = step+1;
              strcpy(simulation[step+1],"End of Simulation !");
            }
          else if(p=strstr(row,"SCENARIODATA"))
            {
              sscanf(p,"%s%d%d%d",temp,&scenDigital,&scenAnalog,&scenInterrupt);
            }
	}

      g_loops = loop;
      //loopPos[loop] = step;
      
      readStatus();
      readSerial();
      readTime();
      fclose(in);
    }
  return;
}    

//====================================
void showScenario(char *fileName)
//====================================
{
  FILE *in;
  char row[SIZE_ROW];
  int i=0;

  wclear(msg);
  in = fopen(fileName,"r");
  if(in == NULL)
    {
      showError("Unable to open scenario file",-1);
      return;
    }
  else
    {
      while (fgets(row,SIZE_ROW,in)!=NULL && i < s_row-1)
        {
          if(strstr(row,"// SCEN"))
	    {
	      i++;
	      wmove(msg,i,1);
	      wprintw(msg,row);
	    }
        }
      if(i == 0)
	{
          wmove(msg,1,1); wprintw(msg,"No scenario data in sketch");
	}
      show(msg);
      fclose(in);
    }
  return;
}

//====================================
void selectProj(int projNo,char *projName)
//====================================
{
  FILE *in;
  char row[SIZE_ROW],temp[SIZE_ROW],*p;
  int i=0;

  strcpy(projName,fileDefault);

  in = fopen(fileProjList,"r");
  if(in == NULL)
    {
      showError("Unable to open list conf file",-1);
      return;
    }
  else
    {
      while (fgets(row,SIZE_ROW,in)!=NULL)
        {
	  i++;
	  if(i==projNo) 
	    {
              sscanf(row,"%s",projName);
	    }
        }
      fclose(in);
    }
  return;
}

//====================================
void readMsg(char *fileName)
//====================================
{
  FILE *in;
  char row[SIZE_ROW],temp[SIZE_ROW],*p;
  int i=0,ch;

  wclear(msg);
  in = fopen(fileName,"r");
  if(in == NULL)
    {
      showError("Unable to open msg file",-1);
      return;
    }
  else
    {
      while (fgets(row,SIZE_ROW,in)!=NULL && ch != 'x')
	{
          i++;
          // If Conf List File
          if(strstr(fileName,fileProjList) != NULL)
	    {
	      strcpy(temp,row);
	      if(p = strstr(temp,".conf")) strcpy(p,"\0");
	      if(strstr(row,currentConf))
		sprintf(row,"> %d %s",i,temp);
	      else
		sprintf(row,"  %d %s",i,temp);
	    }
          if(i < msg_h-2)
	    {
	      wmove(msg,i,1);
	      wprintw(msg,row);
	    }
          else
	    {
	      wmove(msg,msg_h-2,1);
	      wprintw(msg,"press any key (q to quit) >>");
	      wrefresh(msg);
	      ch = getchar();
	      if(ch == 'q')
		{
		  fclose(in);
		  return;
		}
	      wscrl(msg,msg_h-2);
	      i = 1;
	      wmove(msg,i,1);
	      wprintw(msg,row);  
	    }
	}
      show(msg);
      fclose(in);
    }
  return;
}    


//====================================
void init(int mode)
//====================================
{
  int i,j,k;
  
  if(confBoardType == UNO)
    {
      board_w = UNO_W;
      board_h = UNO_H;
      board_x = 0;
      board_y = 0;
    }

  if(confBoardType == MEGA)
    {
      board_w = MEGA_W;
      board_h = MEGA_H;
      board_x = 0;
      board_y = 0;
    }

  dp = 5;
  ap = board_h - 5;

  g_value = 0;

  // Down
  endwin();
  delwin(uno);
  delwin(ser);
  delwin(slog);
  delwin(msg);

  // Up
  initscr();
  clear();
  //noecho();
  cbreak();

  getmaxyx(stdscr,s_row,s_col);
  start_color();
  init_pair(1,COLOR_BLACK,COLOR_BLUE);
  init_pair(2,COLOR_BLACK,COLOR_GREEN);
  init_pair(3,COLOR_BLUE,COLOR_WHITE); 
  init_pair(4,COLOR_RED,COLOR_WHITE); 
  init_pair(5,COLOR_MAGENTA,COLOR_WHITE); 
  init_pair(6,COLOR_WHITE,COLOR_BLACK); 
  init_pair(7,COLOR_WHITE,COLOR_BLUE);
  
  /*     COLOR_BLACK   0 */
  /*     COLOR_RED     1 */
  /*     COLOR_GREEN   2 */
  /*     COLOR_YELLOW  3 */
  /*     COLOR_BLUE    4 */
  /*     COLOR_MAGENTA 5 */
  /*     COLOR_CYAN    6 */
  /*     COLOR_WHITE   7 */

  // Board Window    
  uno=newwin(board_h,board_w,board_x,board_y);
  wbkgd(uno,COLOR_PAIR(UNO_COLOR));
  //box(uno, 0 , 0);

  wmove(uno,dp-1,RF);waddch(uno,ACS_ULCORNER); 
  wmove(uno,dp-1,RF+board_w-3);waddch(uno,ACS_URCORNER); 
  wmove(uno,ap+1,RF);waddch(uno,ACS_LLCORNER); 
  wmove(uno,ap+1,RF+board_w-3);waddch(uno,ACS_LRCORNER); 
  for(i=1;i<board_w-3;i++)
    {
      wmove(uno,dp-1,RF+i);
      waddch(uno,ACS_HLINE);
      wmove(uno,ap+1,RF+i);
      waddch(uno,ACS_HLINE);
    }
  for(i=dp;i<ap+1;i++)
    {
      wmove(uno,i,RF);
      waddch(uno,ACS_VLINE);
      wmove(uno,i,RF+board_w-3);
      waddch(uno,ACS_VLINE);
    }

  // Pin positions on the board
  for(i=0;i<MAX_PIN_DIGITAL_UNO;i++)
    {
      digPinCol[i]  = RF+3+4*(MAX_PIN_DIGITAL_UNO-i-1);
      digPinRow[i]  = dp;
      digIdCol[i]   = RF+2+4*(MAX_PIN_DIGITAL_UNO-i-1);
      digIdRow[i]   = dp+1;
      digActCol[i]  = RF+3+4*(MAX_PIN_DIGITAL_UNO-i-1);
      digActRow[i]  = dp+2;
      digStatCol[i] = RF+3+4*(MAX_PIN_DIGITAL_UNO-i-1);
      digStatRow[i] = dp-2;
    }
  for(i=MAX_PIN_DIGITAL_UNO;i<22;i++)
    {
      digPinCol[i]  = RF+10+4*i;
      digPinRow[i]  = dp;
      digIdCol[i]   = RF+9+4*i;
      digIdRow[i]   = dp+1;
      digActCol[i]  = RF+10+4*i;
      digActRow[i]  = dp+2;
      digStatCol[i] = RF+10+4*i;
      digStatRow[i] = dp-2;
    }
  if(confBoardType == MEGA)
    {
      j = dp+3;
      for(i=22;i<=max_digPin;i=i+2)
	{
	  j++;
	  digPinCol[i]  = board_w - 35;
	  digPinRow[i]  = j; 
	  digIdCol[i]   = board_w - 39;
	  digIdRow[i]   = j; 
	  digActCol[i]  = board_w - 41;
	  digActRow[i]  = j; 
	  digStatCol[i] = board_w - 32;
	  digStatRow[i] = j; 
	}
      j = dp+3;
      for(i=23;i<=max_digPin;i=i+2)
	{
	  j++;
	  digPinCol[i]  = board_w - 11;
	  digPinRow[i]  = j; 
	  digIdCol[i]   = board_w -  9;
	  digIdRow[i]   = j; 
	  digActCol[i]  = board_w -  6;
	  digActRow[i]  = j; 
	  digStatCol[i] = board_w - 14;
	  digStatRow[i] = j; 
	}
    }
  for(i=0;i<=max_anaPin;i++) anaPinCol[i] = RF+27+5*i;

  for(i=0;i<=max_digPin;i++){wmove(uno,digIdRow[i],digIdCol[i]); wprintw(uno,"%2d",i);}
  for(i=0;i<=max_digPin;i++){wmove(uno,digPinRow[i],digPinCol[i]); waddch(uno,ACS_BULLET);}
  //for(i=0;i<=max_digPin;i++){wmove(uno,digActRow[i],digActCol[i]); wprintw(uno,"a");}
  //for(i=0;i<=max_digPin;i++){wmove(uno,digStatRow[i],digStatCol[i]);wprintw(uno,"s");}

  for(i=0;i<=max_anaPin;i++){wmove(uno,ap-1,anaPinCol[i]-1); wprintw(uno,"A%1d",i);}
  for(i=0;i<=max_anaPin;i++){wmove(uno,ap,anaPinCol[i]); waddch(uno,ACS_BULLET);}

  show(uno);

  if(mode == 0) // side by side
    {
      msg_h = s_row - board_h;
      msg_w = board_w;
      msg_x = board_h;
      msg_y = 0;

      log_h = s_row;
      log_w = LOG_W;
      log_x = 0;
      log_y = board_w;   
      
      ser_h = s_row;
      ser_w = s_col - board_w - log_w;
      ser_x = 0;
      ser_y = board_w+log_w;
    }

  if(mode == 1) // 50 on 50
    {
      msg_h = s_row - board_h;
      msg_w = board_w;
      msg_x = board_h;
      msg_y = 0;

      log_h = s_row/2;
      log_w = s_col-uno_w;
      log_x = 0;
      log_y = board_w;   

      ser_h = s_row/2+1;
      ser_w = s_col-board_w;
      ser_x = s_row/2;
      ser_y = board_w;
    }

  if(mode == 2) // 90 on 10
    {
      msg_h = s_row - board_h;
      msg_w = board_w;
      msg_x = board_h;
      msg_y = 0;

      log_h = s_row-10;
      log_w = s_col-board_w;
      log_x = 0;
      log_y = board_w;   

      ser_h = 10;
      ser_w = s_col-board_w;
      ser_x = log_h;
      ser_y = board_w;
    }

  if(mode == 3) // 10 on 90
    {      
      msg_h = s_row - board_h;
      msg_w = board_w;
      msg_x = board_h;
      msg_y = 0;
      
      log_h = 10;
      log_w = s_col-board_w;
      log_x = 0;
      log_y = board_w;   

      ser_h = s_row-10;
      ser_w = s_col-board_w;
      ser_x = log_h;
      ser_y = board_w;
    }

  if(mode == 4) // big message to the right. Log on Ser
    {      
      msg_h = s_row;
      msg_w = s_col - board_w;
      msg_x = 0;
      msg_y = board_w;
      
      log_h = s_row-board_h-10;
      log_w = board_w;
      log_x = board_h;
      log_y = 0;   

      ser_h = s_row-board_h-log_h;
      ser_w = board_w;
      ser_x = log_h+board_h;
      ser_y = 0;
    }

  if(mode == 5) // big message to the right. Log and Ser side by side
    {      
      msg_h = s_row;
      msg_w = s_col - board_w;
      msg_x = 0;
      msg_y = board_w;
      
      log_h = s_row-board_h;
      log_w = board_w/2;
      log_x = board_h;
      log_y = 0;   

      ser_h = s_row-board_h;
      ser_w = board_w/2;
      ser_x = board_h;
      ser_y = log_w;
    }

  msg=newwin(msg_h,msg_w,msg_x,msg_y);
  scrollok(msg,true);
  wbkgd(msg,COLOR_PAIR(MSG_COLOR));
  show(msg);

  slog=newwin(log_h,log_w,log_x,log_y);
  scrollok(slog,true);
  wbkgd(slog,COLOR_PAIR(LOG_COLOR));
  show(slog); 

  ser=newwin(ser_h,ser_w,ser_x,ser_y);
  scrollok(ser,true);
  wbkgd(ser,COLOR_PAIR(SER_COLOR));
  show(ser);

  for(i=0;i<log_w;i++)logBlankRow[i] = ' ';logBlankRow[i]='\0';
  for(i=0;i<ser_w;i++)serBlankRow[i] = ' ';serBlankRow[i]='\0';

}

//====================================
int  countRowsInFile(char *fileName)
//====================================
{
  FILE *in;
  char row[SIZE_ROW];
  int res=0;
  
  in = fopen(fileName,"r");
  if(in == NULL)
    {
      showError("countRowsInFile: Unable to open file",-1);
      return(999);
    }
  else
    {
      while (fgets(row,SIZE_ROW,in)!=NULL)
        {
	  res++;
	}
      fclose(in);
      return(res);
    }
  return(999);
}
//====================================
void anyErrors()
//====================================
{
  int x;
  char syscom[200];
  
  g_existError = NO;
  x = system("rm temp.txt");
  sprintf(syscom,"cat %s %s %s> %s",fileError,fileServError,fileCopyError,fileTemp);
  x = system(syscom); 
  x = countRowsInFile(fileTemp);
  if(x > 0 && x != 999)g_existError = YES;
  if(x == 999)putMsg(2,"Unable to read error file");
  show(uno);
}


//====================================
int loadSketch(char sketch[])
//====================================
{
  int x,ch,res;
  char syscom[120];

  sprintf(syscom,"cp %s %s > %s 2>&1;",sketch,fileServSketch,fileCopyError);
  x=system(syscom);
  strcpy(confSketchFile,sketch);
  sprintf(syscom,"cd servuino; g++ -O2 -o servuino servuino.c > g++.result 2>&1;");
  x=system(syscom);

  x=countRowsInFile(fileServComp);
  if(x > 0)
    {
      readMsg(fileServComp);
      wmove(msg,msg_h-2,1);
      wprintw(msg,"press any key to continue >>");
      wrefresh(msg);
      ch = getchar();
      putMsg(2,"Check your sketch or report an Issue to Simuino");
      return(1);
    }
  readSketchInfo();
  return(0);
}

//====================================
int readScenario()
//====================================
{
  FILE *in;
  char row[SIZE_ROW];
  int step=0,res=0,loop=0;
  
  in = fopen(fileServArduino,"r");
  if(in == NULL)
    {
      showError("readScenario: Unable to open file",-1);
      return(res);
    }
  else
    {
      while (fgets(row,SIZE_ROW,in)!=NULL)
        {
	  if(row[0] == '+')
	    {
	      step++;
	      strcpy(simulation[step],row);
	      if(strstr(row,"Loop"))
		{
		  loop++;
		  loopPos[loop]  = step;
		  loopStep[loop] = step;
		}
	      stepLoop[step] = loop;
	      //printf("%s\n",row);
	    }
	}
      fclose(in);
      g_steps = step;
      g_loops = loop;
      res = step;
    }
  return(res);
}

//====================================
void readTime()
//====================================
{
  FILE *in;
  char row[SIZE_ROW],junk[10];
  int i,step,delay;
  

  for(i=0;i<MAX_STEP;i++)stepDelay[i] = 0;

  in = fopen(fileServTime,"r");
  if(in == NULL)
    {
      showError("readTime: Unable to open file",-1);
      return;
    }
  else
    {
      while (fgets(row,SIZE_ROW,in)!=NULL)
        {
	  if(row[0] == '+')
	    {
	      sscanf(row,"%s %d %d",junk,&step,&delay);
	      stepDelay[step] = delay;
	    }
	}
      fclose(in);
    }
  return;
}

//====================================
int readStatus()
//====================================
{
  FILE *in;
  char row[SIZE_ROW];
  int step=0,res=0;
  
  in = fopen(fileServStatus,"r");
  if(in == NULL)
    {
      showError("readStatus: Unable to open file",-1);
      return(res);
    }
  else
    {
      fgets(row,SIZE_ROW,in);// read first header line in file
      while (fgets(row,SIZE_ROW,in)!=NULL)
        {
	  sscanf(row,"%d",&step);
          //step++;
	  strcpy(status[step],row);
	  //printf("%s\n",row);
	}
      fclose(in);
      res = step;
    }
  return(res);
}

//====================================
void readSerial()
//====================================
{
  FILE *in;
  char *left,*right;  char row[SIZE_ROW],line[SIZE_ROW],value[SIZE_ROW];
  int  step=0,res=0;
  
  in = fopen(fileServSerial,"r");
  if(in == NULL)
    {
      showError("readSerial: Unable to open file",-1);
      return;
    }
  else
    {
      fgets(row,SIZE_ROW,in);// read first header line in file
      while (fgets(row,SIZE_ROW,in)!=NULL)
        {
	  //strcpy(serial[step],row);
	  //printf("%s\n",row);
	  sscanf(row,"%d %s",&step,line);
	  if(strstr(line,"NL")) serialL[step] = 1;
	  else
	    serialL[step] = 0;
	  left = strstr(row,"[");
	  right = strstr(row,"]");
	  //printf("%d %s left=%s right=%s\n",step,line,left,right);
 	  if(right && left)
	    {
	      strcpy(right,"\0"); 
	      strcpy(serial[step],++left);
	      //printf("Serial:%s\n",serial[step]);
	    }
	}
      fclose(in);
    }
  return;
}

//====================================
void displayStatus(char *s)
//====================================
{
  int i;
  char *pch,res[100][240],temp[240];
  int count = 0,step = 0, mode,pin,nd,na,value;
  int digPinValue[MAX_PIN_DIGITAL_MEGA];
  int anaPinValue[MAX_PIN_ANALOG_MEGA];

  for(i=0;i<MAX_PIN_DIGITAL_MEGA;i++)
    {
      digPinValue[i] = 0;
    }
  for(i=0;i<MAX_PIN_ANALOG_MEGA;i++)
    {
      anaPinValue[i] = 0;
    }


  pch = strtok(s,",");
  while (pch != NULL)
    {
      strcpy(res[count],pch);
      count++;
      pch = strtok(NULL, ",");
    }
  
  // ======= Decode =======
  // Step
  sscanf(res[0],"%d",&step);
  
  // Digital Pin Mode
  //printf("len=%d %d %s\n",max_digPin,strlen(res[1]),res[1]);
  for(i=0;i<=max_digPin;i++)
    {
      strcpy(temp,res[1]);
      if(temp[i]=='X')digitalMode[i] = RX;  
      if(temp[i]=='Y')digitalMode[i] = TX; 
      if(temp[i]=='I')digitalMode[i] = INPUT; 
      if(temp[i]=='O')digitalMode[i] = OUTPUT; 
      if(temp[i]=='C')digitalMode[i] = I_CHANGE; 
      if(temp[i]=='R')digitalMode[i] = I_RISING; 
      if(temp[i]=='F')digitalMode[i] = I_FALLING; 
      if(temp[i]=='L')digitalMode[i] = I_LOW; 
      if(temp[i]=='-')digitalMode[i] = 0; 
      if(temp[i]=='Q')digitalMode[i] = WRONG; 
    }
  // printf("%s\n",s);

  // Analog Values
  sscanf(res[2],"%d",&na);
  for(i=0;i<na;i++)
    {
      sscanf(res[4+i*2],"%d",&pin);
      sscanf(res[3+(i+1)*2],"%d",&value);
      anaPinValue[pin] = value;
    }
  // Digital Values
  sscanf(res[3],"%d",&nd);
  for(i=na;i<na+nd;i++)
    {
      sscanf(res[4+i*2],"%d",&pin);
      sscanf(res[3+(i+1)*2],"%d",&value);
      digPinValue[pin] = value;
    }
  
  // ======= Display =======

  // Digital Pin Mode
  for(pin=0;pin<=max_digPin;pin++)
    {
      mode = digitalMode[pin];
      wmove(uno,digPinRow[pin]-1,digPinCol[pin]);
      if(pin < 22)
	waddch(uno,ACS_VLINE);

      wmove(uno,digStatRow[pin],digStatCol[pin]);
      if(mode==INPUT)
	{
	  wprintw(uno,"In");
	}
      else if(mode==OUTPUT)
	{
	  wprintw(uno,"Out");
	}     
      else if(mode==RX)
	{
	  wprintw(uno,"RX");
	}     
      else if(mode==TX)
	{
	  wprintw(uno,"TX");
	}      
      else if(mode==I_RISING)
	{
	  wprintw(uno,"IR");
	}      
      else if(mode==I_FALLING)
	{
	  wprintw(uno,"IF");
	}   
      else if(mode==I_CHANGE)
	{
	  wprintw(uno,"IC");
	}     
      else if(mode==I_LOW)
	{
	  wprintw(uno,"IL");
	}     
      else if(mode==WRONG)
	{
	  wprintw(uno,"???");
	}
      else    
	  wprintw(uno,"   ");  

    }

  // Digital Pin Value
  for(pin=0;pin<=max_digPin;pin++)
    {
      value = digPinValue[pin];
      
      wmove(uno,digPinRow[pin],digPinCol[pin]);
      if(value==HIGH)
	{
	  waddch(uno,ACS_DIAMOND);
	}
      else if(value==LOW)
	{
	  waddch(uno,ACS_BULLET);
	}
      else if(value < 10 && digitalMode[pin] > 0)
	{
	  //wmove(uno,digPinRow[pin],digPinCol[pin]);
	  wprintw(uno,"%1d",value);
	}
      else if(value >= 10 && digitalMode[pin] > 0)
	{
	  wmove(uno,digPinRow[pin],digPinCol[pin]-2);
	  wprintw(uno,"%3d",value);
	}
      else
	{
	  wmove(uno,digPinRow[pin],digPinCol[pin]-2);
	  wprintw(uno,"   ");
	}

    }

  // Analog Pin Value
  for(pin=0;pin<=max_anaPin;pin++)
    {
      value = anaPinValue[pin];
      if(value > 0)
	{
	  wmove(uno,ap,anaPinCol[pin]-3);
	  wprintw(uno,"%4d",value);
	}
    }
  
  show(uno);
}


//====================================
void readSetting()
//====================================
{
  FILE *in;
  char row[120],sketch[120],*p,*q,name[120];

  in = fopen("settings.txt","r");
  if(in == NULL)
    {
      showError("No settings",-1);
      strcpy(currentConf,"default.conf");
      return;
    }
  else
    {
      while (fgets(row,120,in)!=NULL)
	{

	  if(p=strstr(row,"PROJECT:"))
	    {
	      q = strstr(p,":");q++;
	      sscanf(q,"%s",currentConf);
	    }
	  //readSketchName(sketch,name);
	}
    }
}
//====================================
// End of file
//====================================
