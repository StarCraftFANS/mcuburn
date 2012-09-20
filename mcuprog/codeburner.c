#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <newt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <fcntl.h>
#include <libintl.h>
#include <locale.h>
#include <libgen.h>

#define _(String) gettext((String))
#define PROMPT "CODE BURNER V1.0.0"
char errbuf[100];

void initUI()
{
	newtInit();
	newtCls();
}

int closeUI()
{
	newtCls();
	newtFinished();
}
     int start_work(int *mode,struct newtExitStruct *es)
{
        int retval,workprogress=0;
        retval=target_connect();
        if(retval < 0)
        {
        printMessage(errbuf);
        *mode=0;
        workprogress=0;
        }
        else
        {    
       workprogress=1;
       memset(&es,0,sizeof(struct newtExitStruct));
       es->reason=NEWT_EXIT_FDREADY;
       es->u.watch=retval;
       }
        return workprogress;
}

int minuteDelay(int minutes)
{
     if(minutes > 0)   
	newtDelay(60*minutes);
}


int printMessage(char *msg)
{
	if (msg != NULL && msg[0] != '\0') {
		newtWinMessage(_(PROMPT), _("Ok"), msg);
	}
}
int startMCUBurner(int argc,char **argv)
{
        int num=0,action=0;
     struct newtExitStruct es;   
	int i;
	FILE *fp;
	int retval;
	newtGrid grid, subgrid, buttons;
	newtComponent scale,values[5],okay,form, label, answer;
     newtDrawRootText(0,0,"\t\t\t\tCODE BURNER v1.0.0\t\t\t\t");
	scale = newtScale(-1, -1, 30, 100);
	buttons = newtButtonBar(_("Read"), &values[0], _("Write"), &values[1],_("Select Target"), &values[2], _("Save"), &values[3],_("Options"),&values[4],_("Quit"),&okay,NULL);

	subgrid = newtGridVStacked(NEWT_GRID_COMPONENT, scale, NULL);
	label = newtLabel(-1, -1, "Select the Options to Proceed for IC programming ");
	grid = newtGridBasicWindow(label, subgrid, buttons);
	form = newtForm(NULL, NULL, 0);
	newtGridWrappedWindow(grid, _(PROMPT));
	newtGridAddComponentsToForm(grid, form, 1);
	newtGridFree(grid, 1);
	newtFormSetCurrent(form, buttons);
     memset(&es,0,sizeof(struct newtExitStruct));
     es.reason=NEWT_EXIT_HOTKEY;
     do {
	while ((answer = newtRunForm(form,&es)) != okay && (answer != NULL)) {
		if (answer == okay) {
			newtPopWindow();
			newtFormDestroy(form);
			return 1;
		} else {
			break;
		}
          if(action == ICREAD)
          {
                   if(read_hex(buffer,100)> 0)
                             num+=10; 
          }
          else
              {
                  if(action == ICWRITE) 
                  {
                          if(write_hex(buffer,100)> 0)
                                  num+=10; 
                  }
              }
	}    
     if(workprogress == 1)
       newtScaleSet(scale,num);
     else
        {       
           switch(answer)
           {
              case values[0]:
                         action=ICREAD;
                         workprogress=startwork(&action,&es);
                         num=0;
                         break;
              case  values[1]:       
                         action=ICWRITE;
                         workprogress=startwork(&action,&es);
                         scale=0;
                         break;
              default:
                         break;          
           }
      }
     
                    
     
	}while(answer != okay);
	newtPopWindow();
	newtFormDestroy(form);
	return 0;
}


int drawBurner()
{

}

void main(int argc,char **argv)
{
initUI();
printMessage("hello");
startMCUBurner(argc,argv);
closeUI();
}
