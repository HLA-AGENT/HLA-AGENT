
/*******************************************************************************
 testFederateShell.cc

       Associated Header file: testFederateShell.h
*******************************************************************************/

#include <stdio.h>

#ifdef MOTIF
#include <Xm/Xm.h>
#include <Xm/MwmUtil.h>
#include <Xm/MenuShell.h>
#endif /* MOTIF */

#include "UxXt.h"
#include <Xm/Text.h>
#include <Xm/ScrolledW.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/CascadeB.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/MainW.h>
#include <X11/Shell.h>

/*******************************************************************************
       Includes, Defines, and Global variables from the Declarations Editor:
*******************************************************************************/

#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "config.h"

/* Creation functions for other interfaces */
extern swidget create_outputSelectionShell (swidget);
extern swidget create_commandRecordingShell (swidget);
extern swidget create_replayShell (swidget);
extern swidget create_genericPromptDialog (swidget, ServiceID);

/* Swidgets that we will need global access to */
swidget outputSelectionSwidget;
swidget commandRecordingSwidget;
swidget replaySwidget;

/* Various other globals */
FILE              *commandFile;
FILE              *outputFile;       
char               federateHandle[6];
char               federateName[64];
char               federationName[64];
Boolean            outputDisplay;
Boolean            outputRecording;
Boolean            commandRecording;
Boolean            commandReplay;
Boolean            webBrowserVisible;
pid_t              webBrowserPID;

static Boolean  autoTickEnabled;
static const char* const applicationTitle = "Test Federate v1.3";

/* Other Function Prototypes */
void           handleWebBrowserShutdownSignal (int siq);
void           quit (int sig);
void           updateStatus (void);
void           outputMsg (char *message);
void           initializeHelpSystem (void);
void           autoTickEngaged (void);
void           recordCommands (const ServiceID   serviceID,
          const Boolean     extendedMode,
          const int         promptCnt,
          const char* const fields[]);
extern void    do_autoTick (char **fields, int fieldCnt); 
        
static void    invokeUpdateStatus (XtPointer, XtIntervalId*);
static void    stopCommandReplay (void);


#ifndef XKLOADDS
#define XKLOADDS
#endif /* XKLOADDS */

#include "testFederateShell.h"

/*******************************************************************************
       The following function is an event-handler for posting menus.
*******************************************************************************/

void _UxCtestFederateShell::MenuPost(
     Widget wgt, 
     XtPointer client_data, 
     XEvent *event)
{
 Widget menu = (Widget) client_data;
 int  which_button;

 XtVaGetValues( menu, XmNwhichButton, &which_button, NULL );

 if ( event->xbutton.button == which_button )
 {
  XmMenuPosition( menu, (XButtonPressedEvent *) event );
  XtManageChild( menu );
 }
}

Widget recordItem;
Widget replayItem;
Widget objectMenu;
Widget ownershipMenu;
Widget autoTickItem;
Widget statusLine;
Widget separator4;
Widget outputWindowText;

/*******************************************************************************
Auxiliary code from the Declarations Editor:
*******************************************************************************/

void handleWebBrowserShutdownSignal (int sig)
{
  int status;

  if (wait (&status) == -1)
  {
    outputMsg ("Help Subsystem:  Wait Failure, probably due to a failure of Fork.");  
    return;
  }

  webBrowserVisible = False;
  webBrowserPID     = 0;

  signal (SIGCHLD, handleWebBrowserShutdownSignal);
}


void quit (int sig)
{
  signal (SIGCHLD, SIG_IGN);

  if (webBrowserVisible)
  {
    printf ("Shutting Down Web Browser...\n");
    kill (webBrowserPID, SIGTERM);
  }

  exit (0);
}

static void invokeUpdateStatus (XtPointer clientData, XtIntervalId *id)
{
  updateStatus ();
}

void updateStatus ()
{
  static Boolean  first = True;
  char            statusString[250];
  XmString        statusStr;

  if (first)    /* This is a pure, unadulterated hack */
  {
    statusStr = XmStringCreateLtoR (" \n \n", XmFONTLIST_DEFAULT_TAG);
    XtVaSetValues (UxGetWidget (statusLine),
            XmNlabelString, statusStr,
                   NULL);
    XmStringFree (statusStr);

    XtAppAddTimeOut (UxAppContext, 0, invokeUpdateStatus, NULL);

    first = False;
    return;
  }

   if (strcmp (federationName, "") != 0)
    sprintf (statusString, "Federate: %s (%s)    Joined Federation: %s",
             federateName, federateHandle, federationName);
  else
    sprintf (statusString, "Federate is Not Joined");

  if (commandRecording)
    strcat (statusString, "\nCommand Recording in Progress");
  else if (commandReplay)
    strcat (statusString, "\nCommand Replay in Progress");

  if ((outputRecording) && (!outputDisplay))
    strcat (statusString,
            "\nOutput Recording in Progress    Output Display Disabled");
  else if (outputRecording)
    strcat (statusString, "\nOutput Recording in Progress");
  else if (!outputDisplay)
    strcat (statusString, "\nOutput Display Disabled");

  statusStr = XmStringCreateLtoR (statusString, XmFONTLIST_DEFAULT_TAG);
  XtVaSetValues (UxGetWidget (statusLine),
          XmNlabelString, statusStr,
                 NULL);
  XmStringFree (statusStr);
}


void outputMsg (char *message)
{
  static Boolean  firstLine = True;
  XmTextPosition  lastPosition;

  if (outputDisplay)
  {
    if (!firstLine)
    {
      lastPosition = XmTextGetLastPosition (UxGetWidget (outputWindowText));
      XmTextInsert (UxGetWidget (outputWindowText), lastPosition, "\n" );
    }
    else
      firstLine = False;

    lastPosition = XmTextGetLastPosition (UxGetWidget (outputWindowText));
    XmTextInsert (UxGetWidget (outputWindowText), lastPosition, message);
  }

  if (outputRecording)
    fprintf (outputFile, "%s\n", message); 
}


void initializeHelpSystem (void)
{
#ifndef DESIGN_TIME
  pid_t  pid;
#if 0
  if (!webBrowserVisible)
  {
    switch (pid = fork())
    {
      case -1:  /* Error during fork */
        outputMsg ("Can't create new process.");
        return;

      case 0:   /* Child process */
        execlp ("netscape",
                "netscape", "-iconic", "IFSPEC10.html",
                (char *)NULL);

        printf ("Can't execute web browser.\n");
        exit (-1);

      default:  /* Parent process */
        webBrowserVisible = True;
        webBrowserPID     = pid;
        printf ("Child process ID: %i\n", webBrowserPID);
    }
  }
#endif  
#endif
}

void autoTickEngaged (void)
{
  XmString autoTickItemLabel;

  autoTickItemLabel = XmStringCreateLocalized ("Disable Automatic Tick");
  XtVaSetValues (UxGetWidget (autoTickItem),
                 XmNlabelString, autoTickItemLabel,
                 NULL);
  XmStringFree (autoTickItemLabel);

  autoTickEnabled = True;
}



void recordCommands (const ServiceID   serviceID,
       const Boolean     extendedMode,
       const int         promptCnt,
       const char* const fields[])
{
  int i;

  if ((commandRecording) &&
      (serviceID != TICK) && (serviceID != AUTO_TICK))
  {
    /* Output the command data to the file.  Note that there could be */
    /* problems reading this record back in if the field strings      */
    /* contain tab characters.                                        */
    /* Empty fields are denoted by a '~'.                             */
    fprintf (commandFile, "%i\t%i\t%i", serviceID, extendedMode, promptCnt);
    for (i = 0; i < promptCnt; i++)
      fprintf (commandFile, "\t%s",
         (strcmp (fields[i], "") == 0) ? "~" : fields[i]);
    fprintf (commandFile, "\n");
  }
}


/* Read one line from the command file, parse it, execute the
   requested service, then schedule the read of the next line.
   Doing this one line at a time allows for federateAmbassador
   calls to be serviced while playback is occurring.          */  
void replayCommands (XtPointer clientData, XtIntervalId  *id)
{
#define MAX_COMMAND_LENGTH    500
  char       command[MAX_COMMAND_LENGTH];
  int        length;
  char      *cmdArgument;
  int        i;
  ServiceID  serviceID;
  int        extendedMode;
  int        fieldCnt;
  char     **fields;

  if (commandFile != NULL)
  {
    if (fgets (command, MAX_COMMAND_LENGTH, commandFile) != NULL)
    {
      cmdArgument = strtok (command, "\t\n");
      sscanf (cmdArgument, "%i", &serviceID);

      cmdArgument = strtok (NULL, "\t\n");
      sscanf (cmdArgument, "%i", &extendedMode);

      cmdArgument = strtok (NULL, "\t\n");
      sscanf (cmdArgument, "%i", &fieldCnt);

      /* Use the fieldCnt above, and read in the fields */
      fields = (char **)malloc (fieldCnt * sizeof (char *));
      for (i = 0; i < fieldCnt; i++)
      {
        /* Empty fields are denoted by a '~'.  */
 cmdArgument = strtok (NULL, "\t\n");
        if (strcmp (cmdArgument, "~") == 0)
        {
          fields[i] = (char *)malloc (1);
          strcpy (fields[i], "");
        }
        else
        {
          fields[i] = (char *)malloc (strlen (cmdArgument) + 1);
          strcpy (fields[i], cmdArgument);
        }
      }

      /* Now that we've constructed the fields array, call the service */
      if (extendedMode)
 promptRecs[serviceID].extendedForm->service (fields, fieldCnt);
      else
 promptRecs[serviceID].service (fields, fieldCnt);

      /* Release the storage used by the fields array */
      for (i = 0; i < fieldCnt; i++)
 free (fields[i]);
      free (fields);

      /* Schedule the next command read. */
      XtAppAddTimeOut (UxAppContext, 0, replayCommands, NULL);
    }
    else  /* End of File */
    {
      stopCommandReplay ();
      return;
    }
  }
}


static void stopCommandReplay (void)
{
  XmString replayItemLabel;

  fclose (commandFile);
  commandFile   = NULL;
  commandReplay = False;

  replayItemLabel = XmStringCreateLocalized ("Replay File...");
  XtVaSetValues (UxGetWidget (replayItem),
                 XmNlabelString, replayItemLabel,
                 NULL);
  XmStringFree (replayItemLabel);


  /* We are no longer in "Command Replay" mode, so get rid of the
     status line that indicated that we were. */

  updateStatus ();


  /* Re-enable "Start Command Recording..." on the "File" menu. */

  XtVaSetValues (UxGetWidget (recordItem),
                 XmNsensitive, True,
                 NULL);
}

/*******************************************************************************
       The following are callback functions.
*******************************************************************************/

void  _UxCtestFederateShell::activateCB_recordItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 XmString recordItemLabel;
 
 if (commandRecording)  /* Stop the current recording */
 {
   fclose (commandFile);
   commandFile      = NULL; 
   commandRecording = False;
 
   recordItemLabel = XmStringCreateLocalized ("Start Command Recording...");
   XtVaSetValues (UxGetWidget (recordItem),
                  XmNlabelString, recordItemLabel,
                  NULL);
   XmStringFree (recordItemLabel);
 
 
   /* We are no longer in "Command Record" mode, so get rid of the
      status line that indicated that we were. */
 
   updateStatus ();
 
 
   /* Re-enable "File Replay" on the "File" menu. */
 
   XtVaSetValues (UxGetWidget (replayItem),
                  XmNsensitive, True,
                  NULL);
 }
 
 else  /* Start a new recording.  Note that we don't actually start */
       /* the recording until the user selects "Ok" in the Command  */
 {     /* Recording dialog box. */
   if (commandRecordingSwidget == NULL)
     commandRecordingSwidget = create_commandRecordingShell (NULL);
 
   UxPopupInterface (commandRecordingSwidget, no_grab);
 }
}

void  _UxCtestFederateShell::Wrap_activateCB_recordItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_recordItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_replayItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 if (commandReplay)  /* Stop the current replay */
   stopCommandReplay (); 
 else  /* Start a new replay.  Note that we don't actually start    */
       /* the replay until the user selects "Ok" in the Command  */
 {     /* Replay dialog box. */
   if (replaySwidget == NULL)
     replaySwidget = create_replayShell (NULL);
 
   UxPopupInterface (replaySwidget, no_grab);
 }
}

void  _UxCtestFederateShell::Wrap_activateCB_replayItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_replayItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_outputSelectionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 if (outputSelectionSwidget == NULL)
   outputSelectionSwidget = create_outputSelectionShell (NULL);
 UxPopupInterface (outputSelectionSwidget, no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_outputSelectionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_outputSelectionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_quitItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 quit( 0 );
}

void  _UxCtestFederateShell::Wrap_activateCB_quitItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_quitItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_createFedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CREATE_FEDEX), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_createFedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_createFedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_destroyFedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DESTROY_FEDEX), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_destroyFedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_destroyFedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_joinFedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), JOIN_FEDEX), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_joinFedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_joinFedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_resignItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RESIGN_FEDEX), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_resignItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_resignItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_registerSyncPointItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REGISTER_SYNC_POINT), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_registerSyncPointItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_registerSyncPointItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_syncPointAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SYNC_POINT_ACHIEVED), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_syncPointAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_syncPointAchievedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestFederationSaveItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_FEDERATION_SAVE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestFederationSaveItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestFederationSaveItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_federateSaveBegunItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FEDERATE_SAVE_BEGUN), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_federateSaveBegunItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_federateSaveBegunItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_federateSaveAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FEDERATE_SAVE_ACHIEVED), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_federateSaveAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_federateSaveAchievedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_federateSaveNotAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FEDERATE_SAVE_NOT_ACHIEVED), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_federateSaveNotAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_federateSaveNotAchievedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestRestoreItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_RESTORE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestRestoreItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestRestoreItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_restoreAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RESTORE_ACHIEVED), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_restoreAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_restoreAchievedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_restoreNotAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RESTORE_NOT_ACHIEVED), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_restoreNotAchievedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_restoreNotAchievedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_publishObjItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), PUBLISH_OBJ_CLASS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_publishObjItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_publishObjItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_unpublishObjItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNPUBLISH_OBJ_CLASS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_unpublishObjItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_unpublishObjItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_publishInteractionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), PUBLISH_INTERACTION_CLASS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_publishInteractionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_publishInteractionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_unpublishInteractionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNPUBLISH_INTERACTION_CLASS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_unpublishInteractionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_unpublishInteractionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_subscribeObjectClassItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_OBJ_CLASS_ATTR), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_subscribeObjectClassItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_subscribeObjectClassItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_unsubscribeObjectClassItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_OBJ_CLASS_ATTR), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_unsubscribeObjectClassItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_unsubscribeObjectClassItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_subscribeIntItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_INTERACTION_CLASS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_subscribeIntItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_subscribeIntItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_unsubscribeIntItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_INTERACTION_CLASS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_unsubscribeIntItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_unsubscribeIntItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_registerObjectItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REGISTER_OBJ), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_registerObjectItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_registerObjectItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_updateAttrValuesItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UPDATE_ATTR_VALUES), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_updateAttrValuesItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_updateAttrValuesItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_sendInteractionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SEND_INTERACTION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_sendInteractionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_sendInteractionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_deleteObjectItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DELETE_OBJ), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_deleteObjectItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_deleteObjectItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_localDeleteObjectItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), LOCAL_DELETE_OBJ), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_localDeleteObjectItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_localDeleteObjectItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_attrTransportTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_ATTR_TRANSPORT_TYPE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_attrTransportTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_attrTransportTypeItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_interactionTransportTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_INTERACTION_TRANSPORT_TYPE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_interactionTransportTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_interactionTransportTypeItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestObjectUpdateItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_OBJ_ATTR_VALUE_UPDATE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestObjectUpdateItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestObjectUpdateItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestClassUpdateItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_CLASS_ATTR_VALUE_UPDATE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestClassUpdateItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestClassUpdateItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_uncondAttrOwnDivItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNCONDITIONAL_ATTR_OWNERSHIP_DIVESTITURE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_uncondAttrOwnDivItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_uncondAttrOwnDivItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_negAttrOwnDivItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NEGOTIATED_ATTR_OWNERSHIP_DIVESTITURE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_negAttrOwnDivItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_negAttrOwnDivItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_attrOwnAcqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_OWNERSHIP_ACQUISITION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_attrOwnAcqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_attrOwnAcqItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_attrOwnAcqIfAvaItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_OWNERSHIP_ACQUISITION_IF_AVAILABLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_attrOwnAcqIfAvaItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_attrOwnAcqIfAvaItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_attrOwnRelResponseItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_OWNERSHIP_RELEASE_RESPONSE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_attrOwnRelResponseItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_attrOwnRelResponseItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_cancelOwnDivItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CANCEL_NEGOTIATED_ATTR_OWNERSHIP_DIVESTITURE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_cancelOwnDivItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_cancelOwnDivItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_cancelOwnAcqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CANCEL_ATTR_OWNERSHIP_ACQUISITION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_cancelOwnAcqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_cancelOwnAcqItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_queryAttrOwnItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), QUERY_ATTR_OWNERSHIP), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_queryAttrOwnItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_queryAttrOwnItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_attrIsOwnedByFederateItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_IS_OWNED_BY_FEDERATE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_attrIsOwnedByFederateItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_attrIsOwnedByFederateItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_enableTimeRegulationItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_TIME_REGULATION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_enableTimeRegulationItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_enableTimeRegulationItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_disableTimeRegulationItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_TIME_REGULATION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_disableTimeRegulationItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_disableTimeRegulationItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_enableTimeConstrainedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_TIME_CONSTRAINED), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_enableTimeConstrainedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_enableTimeConstrainedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_disableTimeConstrainedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_TIME_CONSTRAINED), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_disableTimeConstrainedItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_disableTimeConstrainedItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_timeAdvanceReqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), TIME_ADVANCE_REQUEST), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_timeAdvanceReqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_timeAdvanceReqItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_timeAdvanceReqAvailableItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), TIME_ADVANCE_REQUEST_AVAILABLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_timeAdvanceReqAvailableItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_timeAdvanceReqAvailableItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_nextEventReqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NEXT_EVENT_REQUEST), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_nextEventReqItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_nextEventReqItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_nextEventReqAvailableItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NEXT_EVENT_REQUEST_AVAILABLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_nextEventReqAvailableItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_nextEventReqAvailableItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_flushQueueRequestsItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FLUSH_QUEUE_REQUEST), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_flushQueueRequestsItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_flushQueueRequestsItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_enableAsynchronousDeliveryItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_ASYNCHRONOUS_DELIVERY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_enableAsynchronousDeliveryItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_enableAsynchronousDeliveryItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_disableAsynchronousDeliveryItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_ASYNCHRONOUS_DELIVERY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_disableAsynchronousDeliveryItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_disableAsynchronousDeliveryItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestLBTSitem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_LBTS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestLBTSitem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestLBTSitem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestFederateTimeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_FEDERATE_TIME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestFederateTimeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestFederateTimeItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestMinEventTimeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_MIN_NEXT_EVENT_TIME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestMinEventTimeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestMinEventTimeItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_changeLookaheadItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_LOOKAHEAD), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_changeLookaheadItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_changeLookaheadItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_requestLookaheadItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_LOOKAHEAD), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_requestLookaheadItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_requestLookaheadItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_retractItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RETRACT), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_retractItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_retractItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_changeAttrOrderTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_ATTR_ORDER_TYPE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_changeAttrOrderTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_changeAttrOrderTypeItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_changeInteractionOrderTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_INTERACTION_ORDER_TYPE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_changeInteractionOrderTypeItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_changeInteractionOrderTypeItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_createUpdateRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CREATE_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_createUpdateRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_createUpdateRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_modifyRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NOTIFY_REGION_MODIFICATION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_modifyRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_modifyRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_deleteRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DELETE_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_deleteRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_deleteRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_registerObjWIthRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REGISTER_OBJ_WITH_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_registerObjWIthRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_registerObjWIthRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_associateRegionForUpdatesItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ASSOCIATE_REGION_FOR_UPDATES), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_associateRegionForUpdatesItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_associateRegionForUpdatesItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_unassociateRegionForUPdatesItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNASSOCIATE_REGION_FOR_UPDATES), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_unassociateRegionForUPdatesItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_unassociateRegionForUPdatesItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_subObjAttrWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_OBJ_CLASS_ATTR_WITH_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_subObjAttrWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_subObjAttrWithRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_unsubObjClassWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_OBJ_CLASS_WITH_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_unsubObjClassWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_unsubObjClassWithRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_subIntClassWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_INTERACTION_CLASS_WITH_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_subIntClassWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_subIntClassWithRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_unsubIntClassWithRegion(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_INTERACTION_CLASS_WITH_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_unsubIntClassWithRegion(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_unsubIntClassWithRegion(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_sendInteractionWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SEND_INTERACTION_WITH_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_sendInteractionWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_sendInteractionWithRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_reqClassAttrValueUpdateWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE_WITH_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_reqClassAttrValueUpdateWithRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_reqClassAttrValueUpdateWithRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_setRangeLowerBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SET_RANGE_LOWER_BOUND), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_setRangeLowerBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_setRangeLowerBoundItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_setRangeUpperBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SET_RANGE_UPPER_BOUND), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_setRangeUpperBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_setRangeUpperBoundItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getRangeLowerBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_LOWER_BOUND), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getRangeLowerBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getRangeLowerBoundItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getRangeUpperBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_UPPER_BOUND), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getRangeUpperBoundItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getRangeUpperBoundItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getSpaceHandleForRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_SPACE_HANDLE_FOR_REGION), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getSpaceHandleForRegionItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getSpaceHandleForRegionItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getNumberOfExtentsItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_NUMBER_OF_EXTENTS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getNumberOfExtentsItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getNumberOfExtentsItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getRangeLowerBoundNotificationLimitItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_LOWER_BOUND_NOTIFICATION_LIMIT), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getRangeLowerBoundNotificationLimitItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getRangeLowerBoundNotificationLimitItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getRangeUpperBoundNotificationLimitItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_UPPER_BOUND_NOTIFICATION_LIMIT), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getRangeUpperBoundNotificationLimitItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getRangeUpperBoundNotificationLimitItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getObjClassHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_CLASS_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getObjClassHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getObjClassHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getAttrHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ATTR_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getAttrHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getAttrHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getInteractionClassHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_INTERACTION_CLASS_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getInteractionClassHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getInteractionClassHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getParamaterHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_PARM_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getParamaterHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getParamaterHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getObjectHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getObjectHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getObjectHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getSpaceHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_SPACE_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getSpaceHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getSpaceHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getDimensionHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_DIMENSION_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getDimensionHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getDimensionHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getTransportationHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_TRANSPORTATION_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getTransportationHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getTransportationHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getOrderingHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ORDERING_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getOrderingHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getOrderingHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getObjClassNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_CLASS_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getObjClassNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getObjClassNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getAttrNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ATTR_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getAttrNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getAttrNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getInteractionClassNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_INTERACTION_CLASS_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getInteractionClassNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getInteractionClassNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getParameterNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_PARM_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getParameterNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getParameterNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getObjectNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getObjectNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getObjectNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getSpaceNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_SPACE_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getSpaceNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getSpaceNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getDimensionNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_DIMENSION_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getDimensionNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getDimensionNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getTransportationNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_TRANSPORTATION_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getTransportationNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getTransportationNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getOrderingNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ORDERING_NAME), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getOrderingNameItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getOrderingNameItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getAttrSpaceHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ATTRIBUTE_SPACE_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getAttrSpaceHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getAttrSpaceHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getObjectClassItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_CLASS), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getObjectClassItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getObjectClassItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_getIntSpaceHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_INTERACTION_SPACE_HANDLE), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_getIntSpaceHandleItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_getIntSpaceHandleItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_enableClassRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_CLASS_RELEVANCE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_enableClassRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_enableClassRelevanceAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_disableClassRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_CLASS_RELEVANCE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_disableClassRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_disableClassRelevanceAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_enableAttributeRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_enableAttributeRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_enableAttributeRelevanceAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_disableAttributeRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_disableAttributeRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_disableAttributeRelevanceAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_enableAttributeScopeAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_ATTRIBUTE_SCOPE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_enableAttributeScopeAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_enableAttributeScopeAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_disableAttributeScopeAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_ATTRIBUTE_SCOPE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_disableAttributeScopeAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_disableAttributeScopeAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_enableInteractionRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_INTERACTION_RELEVANCE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_enableInteractionRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_enableInteractionRelevanceAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_disableInteractionRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_INTERACTION_RELEVANCE_ADVISORY), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_disableInteractionRelevanceAdvisorySwitchItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_disableInteractionRelevanceAdvisorySwitchItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_manualTickItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), TICK), no_grab);
}

void  _UxCtestFederateShell::Wrap_activateCB_manualTickItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_manualTickItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_autoTickItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 XmString autoTickItemLabel;
 
 if (autoTickEnabled)  /* Disable autoTick */
 {
   autoTickEnabled = False;
 
   autoTickItemLabel = XmStringCreateLocalized ("Enable Automatic Tick...");
   XtVaSetValues (UxGetWidget (autoTickItem),
                  XmNlabelString, autoTickItemLabel,
                  NULL);
   XmStringFree (autoTickItemLabel);
 
   do_autoTick (NULL, STOP);
 }
 
 else  /* AutoTick Initialization.  Note that we don't actually start */
 {     /* autoTick until the user selects "Ok" in the prompt dialog box */
   UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget),
                                                 AUTO_TICK), no_grab);
 }
}

void  _UxCtestFederateShell::Wrap_activateCB_autoTickItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_autoTickItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_clearWindowItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 XmTextSetString (UxGetWidget (outputWindowText), "");
}

void  _UxCtestFederateShell::Wrap_activateCB_clearWindowItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_clearWindowItem(UxWidget, UxClientData, UxCallbackArg);
}

void  _UxCtestFederateShell::activateCB_insertMarkerItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 {
 XmTextPosition  lastPosition;
 
 lastPosition = XmTextGetLastPosition (UxGetWidget (outputWindowText));
 XmTextInsert (UxGetWidget (outputWindowText), lastPosition, "\n<+>");
 
 }
}

void  _UxCtestFederateShell::Wrap_activateCB_insertMarkerItem(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 _UxCtestFederateShell   *UxContext;
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;
 UxContext = (_UxCtestFederateShell *) _UxCtestFederateShell::UxGetContext(UxWidget);
 UxContext->activateCB_insertMarkerItem(UxWidget, UxClientData, UxCallbackArg);
}

/*******************************************************************************
       The following is the destroyContext callback function.
       It is needed to free the memory allocated by the context.
*******************************************************************************/

static void DelayedDelete( XtPointer obj, XtIntervalId *)
{
 delete ((_UxCtestFederateShell *) obj);
}
void  _UxCtestFederateShell::UxDestroyContextCB(
     Widget wgt, 
     XtPointer cd, 
     XtPointer cb)
{
 Widget                  UxWidget = wgt;
 XtPointer               UxClientData = cd;
 XtPointer               UxCallbackArg = cb;

 ((_UxCtestFederateShell *) UxClientData)->UxThis = NULL;
 XtAppAddTimeOut(UxAppContext, 0, DelayedDelete, UxClientData);
}

/*******************************************************************************
       The 'build_' function creates all the widgets
       using the resource values specified in the Property Editor.
*******************************************************************************/

Widget _UxCtestFederateShell::_build()
{
 Widget  _UxParent;
 Widget  fileMenu_shell;
 Widget  federationMenu_shell;
 Widget  declarationMenu_shell;
 Widget  publicationsPane_shell;
 Widget  subscriptionsPane_shell;
 Widget  objectMenu_shell;
 Widget  ownershipMenu_shell;
 Widget  timeMenu_shell;
 Widget  DDMmenu_shell;
 Widget  regionAssociationPane_shell;
 Widget  subscriptionWithRegionPane_shell;
 Widget  aboutRegionPane_shell;
 Widget  supportMenu_shell;
 Widget  nameToHandlePane_shell;
 Widget  handleToNamePane_shell;
 Widget  advisorySwitchPane_shell;
 Widget  messageWindowMenu_shell;


 // Creation of testFederateShell
 _UxParent = UxParent;
 if ( _UxParent == NULL )
 {
  _UxParent = UxTopLevel;
 }

 testFederateShell = XtVaCreatePopupShell( "testFederateShell",
   applicationShellWidgetClass,
   _UxParent,
   XmNx, 493,
   XmNy, 409,
   XmNwidth, 638,
   XmNheight, 284,
   XmNiconName, "TestFed",
   XmNminHeight, 106,
   XmNminWidth, 520,
   NULL );
 UxPutContext( testFederateShell, (char *) this );
 UxThis = testFederateShell;



 // Creation of mainWindow1
 mainWindow1 = XtVaCreateManagedWidget( "mainWindow1",
   xmMainWindowWidgetClass,
   testFederateShell,
   XmNunitType, XmPIXELS,
   XmNx, 0,
   XmNy, -1,
   XmNwidth, 598,
   XmNheight, 168,
   NULL );
 UxPutContext( mainWindow1, (char *) this );


 // Creation of menubar
 menubar = XtVaCreateManagedWidget( "menubar",
   xmRowColumnWidgetClass,
   mainWindow1,
   XmNrowColumnType, XmMENU_BAR,
   XmNmenuAccelerator, "<KeyUp>F10",
   NULL );
 UxPutContext( menubar, (char *) this );


 // Creation of fileMenu
 fileMenu_shell = XtVaCreatePopupShell ("fileMenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 fileMenu = XtVaCreateWidget( "fileMenu",
   xmRowColumnWidgetClass,
   fileMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( fileMenu, (char *) this );


 // Creation of recordItem
 recordItem = XtVaCreateManagedWidget( "recordItem",
   xmPushButtonWidgetClass,
   fileMenu,
   RES_CONVERT( XmNlabelString, "Start Command Recording..." ),
   RES_CONVERT( XmNmnemonic, "C" ),
   NULL );
 XtAddCallback( recordItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_recordItem,
  (XtPointer) NULL );

 UxPutContext( recordItem, (char *) this );


 // Creation of replayItem
 replayItem = XtVaCreateManagedWidget( "replayItem",
   xmPushButtonWidgetClass,
   fileMenu,
   RES_CONVERT( XmNlabelString, "Replay File..." ),
   RES_CONVERT( XmNmnemonic, "R" ),
   NULL );
 XtAddCallback( replayItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_replayItem,
  (XtPointer) NULL );

 UxPutContext( replayItem, (char *) this );


 // Creation of outputSelectionItem
 outputSelectionItem = XtVaCreateManagedWidget( "outputSelectionItem",
   xmPushButtonWidgetClass,
   fileMenu,
   RES_CONVERT( XmNlabelString, "Output Selection..." ),
   RES_CONVERT( XmNmnemonic, "O" ),
   NULL );
 XtAddCallback( outputSelectionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_outputSelectionItem,
  (XtPointer) NULL );

 UxPutContext( outputSelectionItem, (char *) this );


 // Creation of menu1_p1_b4
 menu1_p1_b4 = XtVaCreateManagedWidget( "menu1_p1_b4",
   xmSeparatorWidgetClass,
   fileMenu,
   NULL );
 UxPutContext( menu1_p1_b4, (char *) this );


 // Creation of quitItem
 quitItem = XtVaCreateManagedWidget( "quitItem",
   xmPushButtonWidgetClass,
   fileMenu,
   RES_CONVERT( XmNlabelString, "Quit" ),
   RES_CONVERT( XmNmnemonic, "Q" ),
   NULL );
 XtAddCallback( quitItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_quitItem,
  (XtPointer) NULL );

 UxPutContext( quitItem, (char *) this );


 // Creation of fileCMenu
 fileCMenu = XtVaCreateManagedWidget( "fileCMenu",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "File" ),
   RES_CONVERT( XmNmnemonic, "F" ),
   XmNsubMenuId, fileMenu,
   NULL );
 UxPutContext( fileCMenu, (char *) this );


 // Creation of federationMenu
 federationMenu_shell = XtVaCreatePopupShell ("federationMenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 federationMenu = XtVaCreateWidget( "federationMenu",
   xmRowColumnWidgetClass,
   federationMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( federationMenu, (char *) this );


 // Creation of createFedItem
 createFedItem = XtVaCreateManagedWidget( "createFedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Create Federation Execution..." ),
   NULL );
 XtAddCallback( createFedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_createFedItem,
  (XtPointer) NULL );

 UxPutContext( createFedItem, (char *) this );


 // Creation of destroyFedItem
 destroyFedItem = XtVaCreateManagedWidget( "destroyFedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Destroy Federation Execution..." ),
   NULL );
 XtAddCallback( destroyFedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_destroyFedItem,
  (XtPointer) NULL );

 UxPutContext( destroyFedItem, (char *) this );


 // Creation of joinFedItem
 joinFedItem = XtVaCreateManagedWidget( "joinFedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Join Federation Execution..." ),
   NULL );
 XtAddCallback( joinFedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_joinFedItem,
  (XtPointer) NULL );

 UxPutContext( joinFedItem, (char *) this );


 // Creation of resignItem
 resignItem = XtVaCreateManagedWidget( "resignItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Resign Federation Execution..." ),
   RES_CONVERT( XmNmnemonic, "R" ),
   NULL );
 XtAddCallback( resignItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_resignItem,
  (XtPointer) NULL );

 UxPutContext( resignItem, (char *) this );


 // Creation of federationMenu_b17
 federationMenu_b17 = XtVaCreateManagedWidget( "federationMenu_b17",
   xmSeparatorWidgetClass,
   federationMenu,
   NULL );
 UxPutContext( federationMenu_b17, (char *) this );


 // Creation of registerSyncPointItem
 registerSyncPointItem = XtVaCreateManagedWidget( "registerSyncPointItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Register Federation Synchronization Point..." ),
   NULL );
 XtAddCallback( registerSyncPointItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_registerSyncPointItem,
  (XtPointer) NULL );

 UxPutContext( registerSyncPointItem, (char *) this );


 // Creation of syncPointAchievedItem
 syncPointAchievedItem = XtVaCreateManagedWidget( "syncPointAchievedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Synchronization Point Achieved..." ),
   NULL );
 XtAddCallback( syncPointAchievedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_syncPointAchievedItem,
  (XtPointer) NULL );

 UxPutContext( syncPointAchievedItem, (char *) this );


 // Creation of federationMenu_b18
 federationMenu_b18 = XtVaCreateManagedWidget( "federationMenu_b18",
   xmSeparatorWidgetClass,
   federationMenu,
   NULL );
 UxPutContext( federationMenu_b18, (char *) this );


 // Creation of requestFederationSaveItem
 requestFederationSaveItem = XtVaCreateManagedWidget( "requestFederationSaveItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Request Federation Save..." ),
   NULL );
 XtAddCallback( requestFederationSaveItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestFederationSaveItem,
  (XtPointer) NULL );

 UxPutContext( requestFederationSaveItem, (char *) this );


 // Creation of federateSaveBegunItem
 federateSaveBegunItem = XtVaCreateManagedWidget( "federateSaveBegunItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Federate Save Begun..." ),
   NULL );
 XtAddCallback( federateSaveBegunItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_federateSaveBegunItem,
  (XtPointer) NULL );

 UxPutContext( federateSaveBegunItem, (char *) this );


 // Creation of federateSaveAchievedItem
 federateSaveAchievedItem = XtVaCreateManagedWidget( "federateSaveAchievedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Federate Save Complete..." ),
   NULL );
 XtAddCallback( federateSaveAchievedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_federateSaveAchievedItem,
  (XtPointer) NULL );

 UxPutContext( federateSaveAchievedItem, (char *) this );


 // Creation of federateSaveNotAchievedItem
 federateSaveNotAchievedItem = XtVaCreateManagedWidget( "federateSaveNotAchievedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Federate Save Not Complete..." ),
   NULL );
 XtAddCallback( federateSaveNotAchievedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_federateSaveNotAchievedItem,
  (XtPointer) NULL );

 UxPutContext( federateSaveNotAchievedItem, (char *) this );


 // Creation of requestRestoreItem
 requestRestoreItem = XtVaCreateManagedWidget( "requestRestoreItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Request Federation Restore..." ),
   NULL );
 XtAddCallback( requestRestoreItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestRestoreItem,
  (XtPointer) NULL );

 UxPutContext( requestRestoreItem, (char *) this );


 // Creation of restoreAchievedItem
 restoreAchievedItem = XtVaCreateManagedWidget( "restoreAchievedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Federate Restore Complete..." ),
   NULL );
 XtAddCallback( restoreAchievedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_restoreAchievedItem,
  (XtPointer) NULL );

 UxPutContext( restoreAchievedItem, (char *) this );


 // Creation of restoreNotAchievedItem
 restoreNotAchievedItem = XtVaCreateManagedWidget( "restoreNotAchievedItem",
   xmPushButtonWidgetClass,
   federationMenu,
   RES_CONVERT( XmNlabelString, "Federate Restore Not Complete..." ),
   NULL );
 XtAddCallback( restoreNotAchievedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_restoreNotAchievedItem,
  (XtPointer) NULL );

 UxPutContext( restoreNotAchievedItem, (char *) this );


 // Creation of federationCMenu
 federationCMenu = XtVaCreateManagedWidget( "federationCMenu",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "Federation" ),
   XmNsubMenuId, federationMenu,
   RES_CONVERT( XmNmnemonic, "d" ),
   NULL );
 UxPutContext( federationCMenu, (char *) this );


 // Creation of declarationMenu
 declarationMenu_shell = XtVaCreatePopupShell ("declarationMenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 declarationMenu = XtVaCreateWidget( "declarationMenu",
   xmRowColumnWidgetClass,
   declarationMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( declarationMenu, (char *) this );


 // Creation of publicationsPane
 publicationsPane_shell = XtVaCreatePopupShell ("publicationsPane_shell",
   xmMenuShellWidgetClass, declarationMenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 publicationsPane = XtVaCreateWidget( "publicationsPane",
   xmRowColumnWidgetClass,
   publicationsPane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( publicationsPane, (char *) this );


 // Creation of publishObjItem
 publishObjItem = XtVaCreateManagedWidget( "publishObjItem",
   xmPushButtonWidgetClass,
   publicationsPane,
   RES_CONVERT( XmNlabelString, "Publish Object Class..." ),
   NULL );
 XtAddCallback( publishObjItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_publishObjItem,
  (XtPointer) NULL );

 UxPutContext( publishObjItem, (char *) this );


 // Creation of unpublishObjItem
 unpublishObjItem = XtVaCreateManagedWidget( "unpublishObjItem",
   xmPushButtonWidgetClass,
   publicationsPane,
   RES_CONVERT( XmNlabelString, "Unpublish Object Class..." ),
   NULL );
 XtAddCallback( unpublishObjItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_unpublishObjItem,
  (XtPointer) NULL );

 UxPutContext( unpublishObjItem, (char *) this );


 // Creation of publishInteractionItem
 publishInteractionItem = XtVaCreateManagedWidget( "publishInteractionItem",
   xmPushButtonWidgetClass,
   publicationsPane,
   RES_CONVERT( XmNlabelString, "Publish Interaction Class..." ),
   NULL );
 XtAddCallback( publishInteractionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_publishInteractionItem,
  (XtPointer) NULL );

 UxPutContext( publishInteractionItem, (char *) this );


 // Creation of unpublishInteractionItem
 unpublishInteractionItem = XtVaCreateManagedWidget( "unpublishInteractionItem",
   xmPushButtonWidgetClass,
   publicationsPane,
   RES_CONVERT( XmNlabelString, "Unpublish Interaction Class..." ),
   NULL );
 XtAddCallback( unpublishInteractionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_unpublishInteractionItem,
  (XtPointer) NULL );

 UxPutContext( unpublishInteractionItem, (char *) this );


 // Creation of publicationsCMenu
 publicationsCMenu = XtVaCreateManagedWidget( "publicationsCMenu",
   xmCascadeButtonWidgetClass,
   declarationMenu,
   RES_CONVERT( XmNlabelString, "Publications" ),
   XmNsubMenuId, publicationsPane,
   RES_CONVERT( XmNmnemonic, "P" ),
   NULL );
 UxPutContext( publicationsCMenu, (char *) this );


 // Creation of subscriptionsPane
 subscriptionsPane_shell = XtVaCreatePopupShell ("subscriptionsPane_shell",
   xmMenuShellWidgetClass, declarationMenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 subscriptionsPane = XtVaCreateWidget( "subscriptionsPane",
   xmRowColumnWidgetClass,
   subscriptionsPane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( subscriptionsPane, (char *) this );


 // Creation of subscribeObjectClassItem
 subscribeObjectClassItem = XtVaCreateManagedWidget( "subscribeObjectClassItem",
   xmPushButtonWidgetClass,
   subscriptionsPane,
   RES_CONVERT( XmNlabelString, "Subscribe Object Class Attributes..." ),
   NULL );
 XtAddCallback( subscribeObjectClassItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_subscribeObjectClassItem,
  (XtPointer) NULL );

 UxPutContext( subscribeObjectClassItem, (char *) this );


 // Creation of unsubscribeObjectClassItem
 unsubscribeObjectClassItem = XtVaCreateManagedWidget( "unsubscribeObjectClassItem",
   xmPushButtonWidgetClass,
   subscriptionsPane,
   RES_CONVERT( XmNlabelString, "Unsubscribe Object Class..." ),
   NULL );
 XtAddCallback( unsubscribeObjectClassItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_unsubscribeObjectClassItem,
  (XtPointer) NULL );

 UxPutContext( unsubscribeObjectClassItem, (char *) this );


 // Creation of subscribeIntItem
 subscribeIntItem = XtVaCreateManagedWidget( "subscribeIntItem",
   xmPushButtonWidgetClass,
   subscriptionsPane,
   RES_CONVERT( XmNlabelString, "Subscribe Interaction Class..." ),
   NULL );
 XtAddCallback( subscribeIntItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_subscribeIntItem,
  (XtPointer) NULL );

 UxPutContext( subscribeIntItem, (char *) this );


 // Creation of unsubscribeIntItem
 unsubscribeIntItem = XtVaCreateManagedWidget( "unsubscribeIntItem",
   xmPushButtonWidgetClass,
   subscriptionsPane,
   RES_CONVERT( XmNlabelString, "Unsubscribe Interaction Class..." ),
   NULL );
 XtAddCallback( unsubscribeIntItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_unsubscribeIntItem,
  (XtPointer) NULL );

 UxPutContext( unsubscribeIntItem, (char *) this );


 // Creation of subscriptionsCMenu
 subscriptionsCMenu = XtVaCreateManagedWidget( "subscriptionsCMenu",
   xmCascadeButtonWidgetClass,
   declarationMenu,
   RES_CONVERT( XmNlabelString, "Subscriptions" ),
   RES_CONVERT( XmNmnemonic, "S" ),
   XmNsubMenuId, subscriptionsPane,
   NULL );
 UxPutContext( subscriptionsCMenu, (char *) this );


 // Creation of declarationCMenu
 declarationCMenu = XtVaCreateManagedWidget( "declarationCMenu",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "Declaration" ),
   RES_CONVERT( XmNmnemonic, "D" ),
   XmNsubMenuId, declarationMenu,
   NULL );
 UxPutContext( declarationCMenu, (char *) this );


 // Creation of objectMenu
 objectMenu_shell = XtVaCreatePopupShell ("objectMenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 objectMenu = XtVaCreateWidget( "objectMenu",
   xmRowColumnWidgetClass,
   objectMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( objectMenu, (char *) this );


 // Creation of registerObjectItem
 registerObjectItem = XtVaCreateManagedWidget( "registerObjectItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Register Object Instance..." ),
   NULL );
 XtAddCallback( registerObjectItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_registerObjectItem,
  (XtPointer) NULL );

 UxPutContext( registerObjectItem, (char *) this );


 // Creation of updateAttrValuesItem
 updateAttrValuesItem = XtVaCreateManagedWidget( "updateAttrValuesItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Update Attribute Values..." ),
   NULL );
 XtAddCallback( updateAttrValuesItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_updateAttrValuesItem,
  (XtPointer) NULL );

 UxPutContext( updateAttrValuesItem, (char *) this );


 // Creation of sendInteractionItem
 sendInteractionItem = XtVaCreateManagedWidget( "sendInteractionItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Send Interaction..." ),
   NULL );
 XtAddCallback( sendInteractionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_sendInteractionItem,
  (XtPointer) NULL );

 UxPutContext( sendInteractionItem, (char *) this );


 // Creation of deleteObjectItem
 deleteObjectItem = XtVaCreateManagedWidget( "deleteObjectItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Delete Object Instance..." ),
   NULL );
 XtAddCallback( deleteObjectItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_deleteObjectItem,
  (XtPointer) NULL );

 UxPutContext( deleteObjectItem, (char *) this );


 // Creation of localDeleteObjectItem
 localDeleteObjectItem = XtVaCreateManagedWidget( "localDeleteObjectItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Local Delete Object Instance..." ),
   NULL );
 XtAddCallback( localDeleteObjectItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_localDeleteObjectItem,
  (XtPointer) NULL );

 UxPutContext( localDeleteObjectItem, (char *) this );


 // Creation of objectsMenu_b18
 objectsMenu_b18 = XtVaCreateManagedWidget( "objectsMenu_b18",
   xmSeparatorWidgetClass,
   objectMenu,
   NULL );
 UxPutContext( objectsMenu_b18, (char *) this );


 // Creation of attrTransportTypeItem
 attrTransportTypeItem = XtVaCreateManagedWidget( "attrTransportTypeItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Change Attribute Transport Type..." ),
   NULL );
 XtAddCallback( attrTransportTypeItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_attrTransportTypeItem,
  (XtPointer) NULL );

 UxPutContext( attrTransportTypeItem, (char *) this );


 // Creation of interactionTransportTypeItem
 interactionTransportTypeItem = XtVaCreateManagedWidget( "interactionTransportTypeItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Change Interaction Transport Type..." ),
   NULL );
 XtAddCallback( interactionTransportTypeItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_interactionTransportTypeItem,
  (XtPointer) NULL );

 UxPutContext( interactionTransportTypeItem, (char *) this );


 // Creation of requestObjectUpdateItem
 requestObjectUpdateItem = XtVaCreateManagedWidget( "requestObjectUpdateItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Request Object Attribute Value Update..." ),
   NULL );
 XtAddCallback( requestObjectUpdateItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestObjectUpdateItem,
  (XtPointer) NULL );

 UxPutContext( requestObjectUpdateItem, (char *) this );


 // Creation of requestClassUpdateItem
 requestClassUpdateItem = XtVaCreateManagedWidget( "requestClassUpdateItem",
   xmPushButtonWidgetClass,
   objectMenu,
   RES_CONVERT( XmNlabelString, "Request Class Attribute Value Update..." ),
   NULL );
 XtAddCallback( requestClassUpdateItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestClassUpdateItem,
  (XtPointer) NULL );

 UxPutContext( requestClassUpdateItem, (char *) this );


 // Creation of objectCMenu
 objectCMenu = XtVaCreateManagedWidget( "objectCMenu",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "Object" ),
   RES_CONVERT( XmNmnemonic, "O" ),
   XmNsubMenuId, objectMenu,
   NULL );
 UxPutContext( objectCMenu, (char *) this );


 // Creation of ownershipMenu
 ownershipMenu_shell = XtVaCreatePopupShell ("ownershipMenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 ownershipMenu = XtVaCreateWidget( "ownershipMenu",
   xmRowColumnWidgetClass,
   ownershipMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( ownershipMenu, (char *) this );


 // Creation of uncondAttrOwnDivItem
 uncondAttrOwnDivItem = XtVaCreateManagedWidget( "uncondAttrOwnDivItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Unconditional Attribute Ownership Divestiture..." ),
   NULL );
 XtAddCallback( uncondAttrOwnDivItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_uncondAttrOwnDivItem,
  (XtPointer) NULL );

 UxPutContext( uncondAttrOwnDivItem, (char *) this );


 // Creation of negAttrOwnDivItem
 negAttrOwnDivItem = XtVaCreateManagedWidget( "negAttrOwnDivItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Negotiated Attribute Ownership Divestiture..." ),
   NULL );
 XtAddCallback( negAttrOwnDivItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_negAttrOwnDivItem,
  (XtPointer) NULL );

 UxPutContext( negAttrOwnDivItem, (char *) this );


 // Creation of attrOwnAcqItem
 attrOwnAcqItem = XtVaCreateManagedWidget( "attrOwnAcqItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Attribute Ownership Acquisition..." ),
   NULL );
 XtAddCallback( attrOwnAcqItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_attrOwnAcqItem,
  (XtPointer) NULL );

 UxPutContext( attrOwnAcqItem, (char *) this );


 // Creation of attrOwnAcqIfAvaItem
 attrOwnAcqIfAvaItem = XtVaCreateManagedWidget( "attrOwnAcqIfAvaItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Attribute Ownership Acquisition If Available..." ),
   NULL );
 XtAddCallback( attrOwnAcqIfAvaItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_attrOwnAcqIfAvaItem,
  (XtPointer) NULL );

 UxPutContext( attrOwnAcqIfAvaItem, (char *) this );


 // Creation of attrOwnRelResponseItem
 attrOwnRelResponseItem = XtVaCreateManagedWidget( "attrOwnRelResponseItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Attribute Ownership Release Response..." ),
   NULL );
 XtAddCallback( attrOwnRelResponseItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_attrOwnRelResponseItem,
  (XtPointer) NULL );

 UxPutContext( attrOwnRelResponseItem, (char *) this );


 // Creation of cancelOwnDivItem
 cancelOwnDivItem = XtVaCreateManagedWidget( "cancelOwnDivItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Cancel Negotiated Attribute Ownership Divestiture..." ),
   NULL );
 XtAddCallback( cancelOwnDivItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_cancelOwnDivItem,
  (XtPointer) NULL );

 UxPutContext( cancelOwnDivItem, (char *) this );


 // Creation of cancelOwnAcqItem
 cancelOwnAcqItem = XtVaCreateManagedWidget( "cancelOwnAcqItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Cancel Attribute Ownership Acquisition..." ),
   NULL );
 XtAddCallback( cancelOwnAcqItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_cancelOwnAcqItem,
  (XtPointer) NULL );

 UxPutContext( cancelOwnAcqItem, (char *) this );


 // Creation of queryAttrOwnItem
 queryAttrOwnItem = XtVaCreateManagedWidget( "queryAttrOwnItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Query Attribute Ownership..." ),
   NULL );
 XtAddCallback( queryAttrOwnItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_queryAttrOwnItem,
  (XtPointer) NULL );

 UxPutContext( queryAttrOwnItem, (char *) this );


 // Creation of attrIsOwnedByFederateItem
 attrIsOwnedByFederateItem = XtVaCreateManagedWidget( "attrIsOwnedByFederateItem",
   xmPushButtonWidgetClass,
   ownershipMenu,
   RES_CONVERT( XmNlabelString, "Is Attribute Owned By Federate..." ),
   NULL );
 XtAddCallback( attrIsOwnedByFederateItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_attrIsOwnedByFederateItem,
  (XtPointer) NULL );

 UxPutContext( attrIsOwnedByFederateItem, (char *) this );


 // Creation of ownershipCMenu
 ownershipCMenu = XtVaCreateManagedWidget( "ownershipCMenu",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "Ownership" ),
   RES_CONVERT( XmNmnemonic, "w" ),
   XmNsubMenuId, ownershipMenu,
   NULL );
 UxPutContext( ownershipCMenu, (char *) this );


 // Creation of timeMenu
 timeMenu_shell = XtVaCreatePopupShell ("timeMenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 timeMenu = XtVaCreateWidget( "timeMenu",
   xmRowColumnWidgetClass,
   timeMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( timeMenu, (char *) this );


 // Creation of enableTimeRegulationItem
 enableTimeRegulationItem = XtVaCreateManagedWidget( "enableTimeRegulationItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Enable Time Regulation ..." ),
   NULL );
 XtAddCallback( enableTimeRegulationItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_enableTimeRegulationItem,
  (XtPointer) NULL );

 UxPutContext( enableTimeRegulationItem, (char *) this );


 // Creation of disableTimeRegulationItem
 disableTimeRegulationItem = XtVaCreateManagedWidget( "disableTimeRegulationItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Disable Time Regulation ..." ),
   NULL );
 XtAddCallback( disableTimeRegulationItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_disableTimeRegulationItem,
  (XtPointer) NULL );

 UxPutContext( disableTimeRegulationItem, (char *) this );


 // Creation of enableTimeConstrainedItem
 enableTimeConstrainedItem = XtVaCreateManagedWidget( "enableTimeConstrainedItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Enable Time Constrained ..." ),
   NULL );
 XtAddCallback( enableTimeConstrainedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_enableTimeConstrainedItem,
  (XtPointer) NULL );

 UxPutContext( enableTimeConstrainedItem, (char *) this );


 // Creation of disableTimeConstrainedItem
 disableTimeConstrainedItem = XtVaCreateManagedWidget( "disableTimeConstrainedItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Disable Time Constrained ..." ),
   NULL );
 XtAddCallback( disableTimeConstrainedItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_disableTimeConstrainedItem,
  (XtPointer) NULL );

 UxPutContext( disableTimeConstrainedItem, (char *) this );


 // Creation of timeMenu_b17
 timeMenu_b17 = XtVaCreateManagedWidget( "timeMenu_b17",
   xmSeparatorWidgetClass,
   timeMenu,
   NULL );
 UxPutContext( timeMenu_b17, (char *) this );


 // Creation of timeAdvanceReqItem
 timeAdvanceReqItem = XtVaCreateManagedWidget( "timeAdvanceReqItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Time Advance Request..." ),
   NULL );
 XtAddCallback( timeAdvanceReqItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_timeAdvanceReqItem,
  (XtPointer) NULL );

 UxPutContext( timeAdvanceReqItem, (char *) this );


 // Creation of timeAdvanceReqAvailableItem
 timeAdvanceReqAvailableItem = XtVaCreateManagedWidget( "timeAdvanceReqAvailableItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Time Advance Request Available..." ),
   NULL );
 XtAddCallback( timeAdvanceReqAvailableItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_timeAdvanceReqAvailableItem,
  (XtPointer) NULL );

 UxPutContext( timeAdvanceReqAvailableItem, (char *) this );


 // Creation of nextEventReqItem
 nextEventReqItem = XtVaCreateManagedWidget( "nextEventReqItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Next Event Request..." ),
   NULL );
 XtAddCallback( nextEventReqItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_nextEventReqItem,
  (XtPointer) NULL );

 UxPutContext( nextEventReqItem, (char *) this );


 // Creation of nextEventReqAvailableItem
 nextEventReqAvailableItem = XtVaCreateManagedWidget( "nextEventReqAvailableItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Next Event Request Available..." ),
   NULL );
 XtAddCallback( nextEventReqAvailableItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_nextEventReqAvailableItem,
  (XtPointer) NULL );

 UxPutContext( nextEventReqAvailableItem, (char *) this );


 // Creation of flushQueueRequestsItem
 flushQueueRequestsItem = XtVaCreateManagedWidget( "flushQueueRequestsItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Flush Queue Request..." ),
   NULL );
 XtAddCallback( flushQueueRequestsItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_flushQueueRequestsItem,
  (XtPointer) NULL );

 UxPutContext( flushQueueRequestsItem, (char *) this );


 // Creation of timeMenu_b8
 timeMenu_b8 = XtVaCreateManagedWidget( "timeMenu_b8",
   xmSeparatorWidgetClass,
   timeMenu,
   NULL );
 UxPutContext( timeMenu_b8, (char *) this );


 // Creation of enableAsynchronousDeliveryItem
 enableAsynchronousDeliveryItem = XtVaCreateManagedWidget( "enableAsynchronousDeliveryItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Enable Asynchronous Delivery..." ),
   NULL );
 XtAddCallback( enableAsynchronousDeliveryItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_enableAsynchronousDeliveryItem,
  (XtPointer) NULL );

 UxPutContext( enableAsynchronousDeliveryItem, (char *) this );


 // Creation of disableAsynchronousDeliveryItem
 disableAsynchronousDeliveryItem = XtVaCreateManagedWidget( "disableAsynchronousDeliveryItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Disable Asynchronous Delivery..." ),
   NULL );
 XtAddCallback( disableAsynchronousDeliveryItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_disableAsynchronousDeliveryItem,
  (XtPointer) NULL );

 UxPutContext( disableAsynchronousDeliveryItem, (char *) this );


 // Creation of timeMenu_b18
 timeMenu_b18 = XtVaCreateManagedWidget( "timeMenu_b18",
   xmSeparatorWidgetClass,
   timeMenu,
   NULL );
 UxPutContext( timeMenu_b18, (char *) this );


 // Creation of requestLBTSitem
 requestLBTSitem = XtVaCreateManagedWidget( "requestLBTSitem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Query LBTS..." ),
   NULL );
 XtAddCallback( requestLBTSitem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestLBTSitem,
  (XtPointer) NULL );

 UxPutContext( requestLBTSitem, (char *) this );


 // Creation of requestFederateTimeItem
 requestFederateTimeItem = XtVaCreateManagedWidget( "requestFederateTimeItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Query Federate Time..." ),
   NULL );
 XtAddCallback( requestFederateTimeItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestFederateTimeItem,
  (XtPointer) NULL );

 UxPutContext( requestFederateTimeItem, (char *) this );


 // Creation of requestMinEventTimeItem
 requestMinEventTimeItem = XtVaCreateManagedWidget( "requestMinEventTimeItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Query Minimum Next Event Time..." ),
   NULL );
 XtAddCallback( requestMinEventTimeItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestMinEventTimeItem,
  (XtPointer) NULL );

 UxPutContext( requestMinEventTimeItem, (char *) this );


 // Creation of changeLookaheadItem
 changeLookaheadItem = XtVaCreateManagedWidget( "changeLookaheadItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Modify Lookahead..." ),
   NULL );
 XtAddCallback( changeLookaheadItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_changeLookaheadItem,
  (XtPointer) NULL );

 UxPutContext( changeLookaheadItem, (char *) this );


 // Creation of requestLookaheadItem
 requestLookaheadItem = XtVaCreateManagedWidget( "requestLookaheadItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Query Lookahead..." ),
   NULL );
 XtAddCallback( requestLookaheadItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_requestLookaheadItem,
  (XtPointer) NULL );

 UxPutContext( requestLookaheadItem, (char *) this );


 // Creation of timeMenu_b20
 timeMenu_b20 = XtVaCreateManagedWidget( "timeMenu_b20",
   xmSeparatorWidgetClass,
   timeMenu,
   NULL );
 UxPutContext( timeMenu_b20, (char *) this );


 // Creation of retractItem
 retractItem = XtVaCreateManagedWidget( "retractItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Retract..." ),
   NULL );
 XtAddCallback( retractItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_retractItem,
  (XtPointer) NULL );

 UxPutContext( retractItem, (char *) this );


 // Creation of changeAttrOrderTypeItem
 changeAttrOrderTypeItem = XtVaCreateManagedWidget( "changeAttrOrderTypeItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Change Attribute Order Type..." ),
   NULL );
 XtAddCallback( changeAttrOrderTypeItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_changeAttrOrderTypeItem,
  (XtPointer) NULL );

 UxPutContext( changeAttrOrderTypeItem, (char *) this );


 // Creation of changeInteractionOrderTypeItem
 changeInteractionOrderTypeItem = XtVaCreateManagedWidget( "changeInteractionOrderTypeItem",
   xmPushButtonWidgetClass,
   timeMenu,
   RES_CONVERT( XmNlabelString, "Change Interaction Order Type..." ),
   NULL );
 XtAddCallback( changeInteractionOrderTypeItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_changeInteractionOrderTypeItem,
  (XtPointer) NULL );

 UxPutContext( changeInteractionOrderTypeItem, (char *) this );


 // Creation of timeCMenu
 timeCMenu = XtVaCreateManagedWidget( "timeCMenu",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "Time" ),
   XmNsubMenuId, timeMenu,
   RES_CONVERT( XmNmnemonic, "T" ),
   NULL );
 UxPutContext( timeCMenu, (char *) this );


 // Creation of DDMmenu
 DDMmenu_shell = XtVaCreatePopupShell ("DDMmenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 DDMmenu = XtVaCreateWidget( "DDMmenu",
   xmRowColumnWidgetClass,
   DDMmenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( DDMmenu, (char *) this );


 // Creation of createUpdateRegionItem
 createUpdateRegionItem = XtVaCreateManagedWidget( "createUpdateRegionItem",
   xmPushButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Create Region ..." ),
   NULL );
 XtAddCallback( createUpdateRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_createUpdateRegionItem,
  (XtPointer) NULL );

 UxPutContext( createUpdateRegionItem, (char *) this );


 // Creation of modifyRegionItem
 modifyRegionItem = XtVaCreateManagedWidget( "modifyRegionItem",
   xmPushButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Modify Region ..." ),
   NULL );
 XtAddCallback( modifyRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_modifyRegionItem,
  (XtPointer) NULL );

 UxPutContext( modifyRegionItem, (char *) this );


 // Creation of deleteRegionItem
 deleteRegionItem = XtVaCreateManagedWidget( "deleteRegionItem",
   xmPushButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Delete Region ..." ),
   NULL );
 XtAddCallback( deleteRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_deleteRegionItem,
  (XtPointer) NULL );

 UxPutContext( deleteRegionItem, (char *) this );


 // Creation of registerObjWIthRegionItem
 registerObjWIthRegionItem = XtVaCreateManagedWidget( "registerObjWIthRegionItem",
   xmPushButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Register Object Instance With Region ..." ),
   NULL );
 XtAddCallback( registerObjWIthRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_registerObjWIthRegionItem,
  (XtPointer) NULL );

 UxPutContext( registerObjWIthRegionItem, (char *) this );


 // Creation of DDMmenu_b7
 DDMmenu_b7 = XtVaCreateManagedWidget( "DDMmenu_b7",
   xmSeparatorWidgetClass,
   DDMmenu,
   NULL );
 UxPutContext( DDMmenu_b7, (char *) this );


 // Creation of regionAssociationPane
 regionAssociationPane_shell = XtVaCreatePopupShell ("regionAssociationPane_shell",
   xmMenuShellWidgetClass, DDMmenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 regionAssociationPane = XtVaCreateWidget( "regionAssociationPane",
   xmRowColumnWidgetClass,
   regionAssociationPane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( regionAssociationPane, (char *) this );


 // Creation of associateRegionForUpdatesItem
 associateRegionForUpdatesItem = XtVaCreateManagedWidget( "associateRegionForUpdatesItem",
   xmPushButtonWidgetClass,
   regionAssociationPane,
   RES_CONVERT( XmNlabelString, "Associate Region for Updates ..." ),
   NULL );
 XtAddCallback( associateRegionForUpdatesItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_associateRegionForUpdatesItem,
  (XtPointer) NULL );

 UxPutContext( associateRegionForUpdatesItem, (char *) this );


 // Creation of unassociateRegionForUPdatesItem
 unassociateRegionForUPdatesItem = XtVaCreateManagedWidget( "unassociateRegionForUPdatesItem",
   xmPushButtonWidgetClass,
   regionAssociationPane,
   RES_CONVERT( XmNlabelString, "Unassociate Region For Updates ..." ),
   NULL );
 XtAddCallback( unassociateRegionForUPdatesItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_unassociateRegionForUPdatesItem,
  (XtPointer) NULL );

 UxPutContext( unassociateRegionForUPdatesItem, (char *) this );


 // Creation of regionAssociationCMenu
 regionAssociationCMenu = XtVaCreateManagedWidget( "regionAssociationCMenu",
   xmCascadeButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Associations" ),
   XmNsubMenuId, regionAssociationPane,
   NULL );
 UxPutContext( regionAssociationCMenu, (char *) this );


 // Creation of subscriptionWithRegionPane
 subscriptionWithRegionPane_shell = XtVaCreatePopupShell ("subscriptionWithRegionPane_shell",
   xmMenuShellWidgetClass, DDMmenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 subscriptionWithRegionPane = XtVaCreateWidget( "subscriptionWithRegionPane",
   xmRowColumnWidgetClass,
   subscriptionWithRegionPane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( subscriptionWithRegionPane, (char *) this );


 // Creation of subObjAttrWithRegionItem
 subObjAttrWithRegionItem = XtVaCreateManagedWidget( "subObjAttrWithRegionItem",
   xmPushButtonWidgetClass,
   subscriptionWithRegionPane,
   RES_CONVERT( XmNlabelString, "Subscribe Object Class Attributes With Region ..." ),
   NULL );
 XtAddCallback( subObjAttrWithRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_subObjAttrWithRegionItem,
  (XtPointer) NULL );

 UxPutContext( subObjAttrWithRegionItem, (char *) this );


 // Creation of unsubObjClassWithRegionItem
 unsubObjClassWithRegionItem = XtVaCreateManagedWidget( "unsubObjClassWithRegionItem",
   xmPushButtonWidgetClass,
   subscriptionWithRegionPane,
   RES_CONVERT( XmNlabelString, "Unsubscribe Object Class With Region ..." ),
   NULL );
 XtAddCallback( unsubObjClassWithRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_unsubObjClassWithRegionItem,
  (XtPointer) NULL );

 UxPutContext( unsubObjClassWithRegionItem, (char *) this );


 // Creation of subIntClassWithRegionItem
 subIntClassWithRegionItem = XtVaCreateManagedWidget( "subIntClassWithRegionItem",
   xmPushButtonWidgetClass,
   subscriptionWithRegionPane,
   RES_CONVERT( XmNlabelString, "Subscribe Interaction Class With Region ..." ),
   NULL );
 XtAddCallback( subIntClassWithRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_subIntClassWithRegionItem,
  (XtPointer) NULL );

 UxPutContext( subIntClassWithRegionItem, (char *) this );


 // Creation of unsubIntClassWithRegion
 unsubIntClassWithRegion = XtVaCreateManagedWidget( "unsubIntClassWithRegion",
   xmPushButtonWidgetClass,
   subscriptionWithRegionPane,
   RES_CONVERT( XmNlabelString, "Unsubscribe Interaction Class With Region ..." ),
   NULL );
 XtAddCallback( unsubIntClassWithRegion, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_unsubIntClassWithRegion,
  (XtPointer) NULL );

 UxPutContext( unsubIntClassWithRegion, (char *) this );


 // Creation of subscriptionWithRegionCMenu
 subscriptionWithRegionCMenu = XtVaCreateManagedWidget( "subscriptionWithRegionCMenu",
   xmCascadeButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Subscription With Region" ),
   XmNsubMenuId, subscriptionWithRegionPane,
   NULL );
 UxPutContext( subscriptionWithRegionCMenu, (char *) this );


 // Creation of DDMmenu_b11
 DDMmenu_b11 = XtVaCreateManagedWidget( "DDMmenu_b11",
   xmSeparatorWidgetClass,
   DDMmenu,
   NULL );
 UxPutContext( DDMmenu_b11, (char *) this );


 // Creation of sendInteractionWithRegionItem
 sendInteractionWithRegionItem = XtVaCreateManagedWidget( "sendInteractionWithRegionItem",
   xmPushButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Send Interaction With Region ..." ),
   NULL );
 XtAddCallback( sendInteractionWithRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_sendInteractionWithRegionItem,
  (XtPointer) NULL );

 UxPutContext( sendInteractionWithRegionItem, (char *) this );


 // Creation of reqClassAttrValueUpdateWithRegionItem
 reqClassAttrValueUpdateWithRegionItem = XtVaCreateManagedWidget( "reqClassAttrValueUpdateWithRegionItem",
   xmPushButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "Request Attribute Value Update With Region ..." ),
   NULL );
 XtAddCallback( reqClassAttrValueUpdateWithRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_reqClassAttrValueUpdateWithRegionItem,
  (XtPointer) NULL );

 UxPutContext( reqClassAttrValueUpdateWithRegionItem, (char *) this );


 // Creation of DDMmenu_b12
 DDMmenu_b12 = XtVaCreateManagedWidget( "DDMmenu_b12",
   xmSeparatorWidgetClass,
   DDMmenu,
   NULL );
 UxPutContext( DDMmenu_b12, (char *) this );


 // Creation of aboutRegionPane
 aboutRegionPane_shell = XtVaCreatePopupShell ("aboutRegionPane_shell",
   xmMenuShellWidgetClass, DDMmenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 aboutRegionPane = XtVaCreateWidget( "aboutRegionPane",
   xmRowColumnWidgetClass,
   aboutRegionPane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( aboutRegionPane, (char *) this );


 // Creation of setRangeLowerBoundItem
 setRangeLowerBoundItem = XtVaCreateManagedWidget( "setRangeLowerBoundItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Set Range Lower Bound..." ),
   NULL );
 XtAddCallback( setRangeLowerBoundItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_setRangeLowerBoundItem,
  (XtPointer) NULL );

 UxPutContext( setRangeLowerBoundItem, (char *) this );


 // Creation of setRangeUpperBoundItem
 setRangeUpperBoundItem = XtVaCreateManagedWidget( "setRangeUpperBoundItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Set Range Upper Bound..." ),
   NULL );
 XtAddCallback( setRangeUpperBoundItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_setRangeUpperBoundItem,
  (XtPointer) NULL );

 UxPutContext( setRangeUpperBoundItem, (char *) this );


 // Creation of getRangeLowerBoundItem
 getRangeLowerBoundItem = XtVaCreateManagedWidget( "getRangeLowerBoundItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Get Range Lower Bound..." ),
   NULL );
 XtAddCallback( getRangeLowerBoundItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getRangeLowerBoundItem,
  (XtPointer) NULL );

 UxPutContext( getRangeLowerBoundItem, (char *) this );


 // Creation of getRangeUpperBoundItem
 getRangeUpperBoundItem = XtVaCreateManagedWidget( "getRangeUpperBoundItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Get Range Upper Bound..." ),
   NULL );
 XtAddCallback( getRangeUpperBoundItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getRangeUpperBoundItem,
  (XtPointer) NULL );

 UxPutContext( getRangeUpperBoundItem, (char *) this );


 // Creation of getSpaceHandleForRegionItem
 getSpaceHandleForRegionItem = XtVaCreateManagedWidget( "getSpaceHandleForRegionItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Get Space Handle For Region..." ),
   NULL );
 XtAddCallback( getSpaceHandleForRegionItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getSpaceHandleForRegionItem,
  (XtPointer) NULL );

 UxPutContext( getSpaceHandleForRegionItem, (char *) this );


 // Creation of getNumberOfExtentsItem
 getNumberOfExtentsItem = XtVaCreateManagedWidget( "getNumberOfExtentsItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Get Number Of Extents..." ),
   NULL );
 XtAddCallback( getNumberOfExtentsItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getNumberOfExtentsItem,
  (XtPointer) NULL );

 UxPutContext( getNumberOfExtentsItem, (char *) this );


 // Creation of getRangeLowerBoundNotificationLimitItem
 getRangeLowerBoundNotificationLimitItem = XtVaCreateManagedWidget( "getRangeLowerBoundNotificationLimitItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Get Range Lower Bound Notification Limit..." ),
   NULL );
 XtAddCallback( getRangeLowerBoundNotificationLimitItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getRangeLowerBoundNotificationLimitItem,
  (XtPointer) NULL );

 UxPutContext( getRangeLowerBoundNotificationLimitItem, (char *) this );


 // Creation of getRangeUpperBoundNotificationLimitItem
 getRangeUpperBoundNotificationLimitItem = XtVaCreateManagedWidget( "getRangeUpperBoundNotificationLimitItem",
   xmPushButtonWidgetClass,
   aboutRegionPane,
   RES_CONVERT( XmNlabelString, "Get Range Upper Bound Notification Limit..." ),
   NULL );
 XtAddCallback( getRangeUpperBoundNotificationLimitItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getRangeUpperBoundNotificationLimitItem,
  (XtPointer) NULL );

 UxPutContext( getRangeUpperBoundNotificationLimitItem, (char *) this );


 // Creation of aboutRegionCMenu
 aboutRegionCMenu = XtVaCreateManagedWidget( "aboutRegionCMenu",
   xmCascadeButtonWidgetClass,
   DDMmenu,
   RES_CONVERT( XmNlabelString, "About Region" ),
   XmNsubMenuId, aboutRegionPane,
   NULL );
 UxPutContext( aboutRegionCMenu, (char *) this );


 // Creation of menubar_top_b2
 menubar_top_b2 = XtVaCreateManagedWidget( "menubar_top_b2",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "DDM" ),
   XmNsubMenuId, DDMmenu,
   RES_CONVERT( XmNmnemonic, "M" ),
   NULL );
 UxPutContext( menubar_top_b2, (char *) this );


 // Creation of supportMenu
 supportMenu_shell = XtVaCreatePopupShell ("supportMenu_shell",
   xmMenuShellWidgetClass, menubar,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 supportMenu = XtVaCreateWidget( "supportMenu",
   xmRowColumnWidgetClass,
   supportMenu_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( supportMenu, (char *) this );


 // Creation of nameToHandlePane
 nameToHandlePane_shell = XtVaCreatePopupShell ("nameToHandlePane_shell",
   xmMenuShellWidgetClass, supportMenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 nameToHandlePane = XtVaCreateWidget( "nameToHandlePane",
   xmRowColumnWidgetClass,
   nameToHandlePane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( nameToHandlePane, (char *) this );


 // Creation of getObjClassHandleItem
 getObjClassHandleItem = XtVaCreateManagedWidget( "getObjClassHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Object Class Handle..." ),
   NULL );
 XtAddCallback( getObjClassHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getObjClassHandleItem,
  (XtPointer) NULL );

 UxPutContext( getObjClassHandleItem, (char *) this );


 // Creation of getAttrHandleItem
 getAttrHandleItem = XtVaCreateManagedWidget( "getAttrHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Attribute Handle..." ),
   NULL );
 XtAddCallback( getAttrHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getAttrHandleItem,
  (XtPointer) NULL );

 UxPutContext( getAttrHandleItem, (char *) this );


 // Creation of getInteractionClassHandleItem
 getInteractionClassHandleItem = XtVaCreateManagedWidget( "getInteractionClassHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Interaction Class Handle..." ),
   NULL );
 XtAddCallback( getInteractionClassHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getInteractionClassHandleItem,
  (XtPointer) NULL );

 UxPutContext( getInteractionClassHandleItem, (char *) this );


 // Creation of getParamaterHandleItem
 getParamaterHandleItem = XtVaCreateManagedWidget( "getParamaterHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Parameter Handle..." ),
   NULL );
 XtAddCallback( getParamaterHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getParamaterHandleItem,
  (XtPointer) NULL );

 UxPutContext( getParamaterHandleItem, (char *) this );


 // Creation of getObjectHandleItem
 getObjectHandleItem = XtVaCreateManagedWidget( "getObjectHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Object Instance Handle ..." ),
   NULL );
 XtAddCallback( getObjectHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getObjectHandleItem,
  (XtPointer) NULL );

 UxPutContext( getObjectHandleItem, (char *) this );


 // Creation of getSpaceHandleItem
 getSpaceHandleItem = XtVaCreateManagedWidget( "getSpaceHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Routing Space Handle..." ),
   NULL );
 XtAddCallback( getSpaceHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getSpaceHandleItem,
  (XtPointer) NULL );

 UxPutContext( getSpaceHandleItem, (char *) this );


 // Creation of getDimensionHandleItem
 getDimensionHandleItem = XtVaCreateManagedWidget( "getDimensionHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Dimension Handle..." ),
   NULL );
 XtAddCallback( getDimensionHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getDimensionHandleItem,
  (XtPointer) NULL );

 UxPutContext( getDimensionHandleItem, (char *) this );


 // Creation of getTransportationHandleItem
 getTransportationHandleItem = XtVaCreateManagedWidget( "getTransportationHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Transportation Handle..." ),
   NULL );
 XtAddCallback( getTransportationHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getTransportationHandleItem,
  (XtPointer) NULL );

 UxPutContext( getTransportationHandleItem, (char *) this );


 // Creation of getOrderingHandleItem
 getOrderingHandleItem = XtVaCreateManagedWidget( "getOrderingHandleItem",
   xmPushButtonWidgetClass,
   nameToHandlePane,
   RES_CONVERT( XmNlabelString, "Get Ordering Handle..." ),
   NULL );
 XtAddCallback( getOrderingHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getOrderingHandleItem,
  (XtPointer) NULL );

 UxPutContext( getOrderingHandleItem, (char *) this );


 // Creation of nameToHandleCMenu
 nameToHandleCMenu = XtVaCreateManagedWidget( "nameToHandleCMenu",
   xmCascadeButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Name to Handle" ),
   XmNsubMenuId, nameToHandlePane,
   NULL );
 UxPutContext( nameToHandleCMenu, (char *) this );


 // Creation of handleToNamePane
 handleToNamePane_shell = XtVaCreatePopupShell ("handleToNamePane_shell",
   xmMenuShellWidgetClass, supportMenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 handleToNamePane = XtVaCreateWidget( "handleToNamePane",
   xmRowColumnWidgetClass,
   handleToNamePane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( handleToNamePane, (char *) this );


 // Creation of getObjClassNameItem
 getObjClassNameItem = XtVaCreateManagedWidget( "getObjClassNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Object Class Name..." ),
   NULL );
 XtAddCallback( getObjClassNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getObjClassNameItem,
  (XtPointer) NULL );

 UxPutContext( getObjClassNameItem, (char *) this );


 // Creation of getAttrNameItem
 getAttrNameItem = XtVaCreateManagedWidget( "getAttrNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Attribute Name..." ),
   NULL );
 XtAddCallback( getAttrNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getAttrNameItem,
  (XtPointer) NULL );

 UxPutContext( getAttrNameItem, (char *) this );


 // Creation of getInteractionClassNameItem
 getInteractionClassNameItem = XtVaCreateManagedWidget( "getInteractionClassNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Interaction Class Name..." ),
   NULL );
 XtAddCallback( getInteractionClassNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getInteractionClassNameItem,
  (XtPointer) NULL );

 UxPutContext( getInteractionClassNameItem, (char *) this );


 // Creation of getParameterNameItem
 getParameterNameItem = XtVaCreateManagedWidget( "getParameterNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Parameter Name..." ),
   NULL );
 XtAddCallback( getParameterNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getParameterNameItem,
  (XtPointer) NULL );

 UxPutContext( getParameterNameItem, (char *) this );


 // Creation of getObjectNameItem
 getObjectNameItem = XtVaCreateManagedWidget( "getObjectNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Object Instance Name ..." ),
   NULL );
 XtAddCallback( getObjectNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getObjectNameItem,
  (XtPointer) NULL );

 UxPutContext( getObjectNameItem, (char *) this );


 // Creation of getSpaceNameItem
 getSpaceNameItem = XtVaCreateManagedWidget( "getSpaceNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Routing Space Name..." ),
   NULL );
 XtAddCallback( getSpaceNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getSpaceNameItem,
  (XtPointer) NULL );

 UxPutContext( getSpaceNameItem, (char *) this );


 // Creation of getDimensionNameItem
 getDimensionNameItem = XtVaCreateManagedWidget( "getDimensionNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Dimension Name..." ),
   NULL );
 XtAddCallback( getDimensionNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getDimensionNameItem,
  (XtPointer) NULL );

 UxPutContext( getDimensionNameItem, (char *) this );


 // Creation of getTransportationNameItem
 getTransportationNameItem = XtVaCreateManagedWidget( "getTransportationNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Transportation Name..." ),
   NULL );
 XtAddCallback( getTransportationNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getTransportationNameItem,
  (XtPointer) NULL );

 UxPutContext( getTransportationNameItem, (char *) this );


 // Creation of getOrderingNameItem
 getOrderingNameItem = XtVaCreateManagedWidget( "getOrderingNameItem",
   xmPushButtonWidgetClass,
   handleToNamePane,
   RES_CONVERT( XmNlabelString, "Get Ordering Name..." ),
   NULL );
 XtAddCallback( getOrderingNameItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getOrderingNameItem,
  (XtPointer) NULL );

 UxPutContext( getOrderingNameItem, (char *) this );


 // Creation of handleToNameCMenu
 handleToNameCMenu = XtVaCreateManagedWidget( "handleToNameCMenu",
   xmCascadeButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Handle to Name" ),
   XmNsubMenuId, handleToNamePane,
   NULL );
 UxPutContext( handleToNameCMenu, (char *) this );


 // Creation of supportMenu_b18
 supportMenu_b18 = XtVaCreateManagedWidget( "supportMenu_b18",
   xmSeparatorWidgetClass,
   supportMenu,
   NULL );
 UxPutContext( supportMenu_b18, (char *) this );


 // Creation of getAttrSpaceHandleItem
 getAttrSpaceHandleItem = XtVaCreateManagedWidget( "getAttrSpaceHandleItem",
   xmPushButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Get Attribute Routing Space Handle..." ),
   NULL );
 XtAddCallback( getAttrSpaceHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getAttrSpaceHandleItem,
  (XtPointer) NULL );

 UxPutContext( getAttrSpaceHandleItem, (char *) this );


 // Creation of getObjectClassItem
 getObjectClassItem = XtVaCreateManagedWidget( "getObjectClassItem",
   xmPushButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Get Object Class ..." ),
   NULL );
 XtAddCallback( getObjectClassItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getObjectClassItem,
  (XtPointer) NULL );

 UxPutContext( getObjectClassItem, (char *) this );


 // Creation of getIntSpaceHandleItem
 getIntSpaceHandleItem = XtVaCreateManagedWidget( "getIntSpaceHandleItem",
   xmPushButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Get Interaction Routing Space Handle..." ),
   NULL );
 XtAddCallback( getIntSpaceHandleItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_getIntSpaceHandleItem,
  (XtPointer) NULL );

 UxPutContext( getIntSpaceHandleItem, (char *) this );


 // Creation of supportMenu_b19
 supportMenu_b19 = XtVaCreateManagedWidget( "supportMenu_b19",
   xmSeparatorWidgetClass,
   supportMenu,
   NULL );
 UxPutContext( supportMenu_b19, (char *) this );


 // Creation of advisorySwitchPane
 advisorySwitchPane_shell = XtVaCreatePopupShell ("advisorySwitchPane_shell",
   xmMenuShellWidgetClass, supportMenu,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 advisorySwitchPane = XtVaCreateWidget( "advisorySwitchPane",
   xmRowColumnWidgetClass,
   advisorySwitchPane_shell,
   XmNrowColumnType, XmMENU_PULLDOWN,
   NULL );
 UxPutContext( advisorySwitchPane, (char *) this );


 // Creation of enableClassRelevanceAdvisorySwitchItem
 enableClassRelevanceAdvisorySwitchItem = XtVaCreateManagedWidget( "enableClassRelevanceAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Enable Class Relevance Advisory Switch..." ),
   NULL );
 XtAddCallback( enableClassRelevanceAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_enableClassRelevanceAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( enableClassRelevanceAdvisorySwitchItem, (char *) this );


 // Creation of disableClassRelevanceAdvisorySwitchItem
 disableClassRelevanceAdvisorySwitchItem = XtVaCreateManagedWidget( "disableClassRelevanceAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Disable Class Relevance Advisory Switch..." ),
   NULL );
 XtAddCallback( disableClassRelevanceAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_disableClassRelevanceAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( disableClassRelevanceAdvisorySwitchItem, (char *) this );


 // Creation of enableAttributeRelevanceAdvisorySwitchItem
 enableAttributeRelevanceAdvisorySwitchItem = XtVaCreateManagedWidget( "enableAttributeRelevanceAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Enable Attribute Relevance Advisory Switch..." ),
   NULL );
 XtAddCallback( enableAttributeRelevanceAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_enableAttributeRelevanceAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( enableAttributeRelevanceAdvisorySwitchItem, (char *) this );


 // Creation of disableAttributeRelevanceAdvisorySwitchItem
 disableAttributeRelevanceAdvisorySwitchItem = XtVaCreateManagedWidget( "disableAttributeRelevanceAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Disable Attribute Relevance Advisory Switch..." ),
   NULL );
 XtAddCallback( disableAttributeRelevanceAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_disableAttributeRelevanceAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( disableAttributeRelevanceAdvisorySwitchItem, (char *) this );


 // Creation of enableAttributeScopeAdvisorySwitchItem
 enableAttributeScopeAdvisorySwitchItem = XtVaCreateManagedWidget( "enableAttributeScopeAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Enable Attribute Scope Advisory Switch..." ),
   NULL );
 XtAddCallback( enableAttributeScopeAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_enableAttributeScopeAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( enableAttributeScopeAdvisorySwitchItem, (char *) this );


 // Creation of disableAttributeScopeAdvisorySwitchItem
 disableAttributeScopeAdvisorySwitchItem = XtVaCreateManagedWidget( "disableAttributeScopeAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Disable Attribute Scope Advisory Switch..." ),
   NULL );
 XtAddCallback( disableAttributeScopeAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_disableAttributeScopeAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( disableAttributeScopeAdvisorySwitchItem, (char *) this );


 // Creation of enableInteractionRelevanceAdvisorySwitchItem
 enableInteractionRelevanceAdvisorySwitchItem = XtVaCreateManagedWidget( "enableInteractionRelevanceAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Enable Interaction Relevance Advisory Switch..." ),
   NULL );
 XtAddCallback( enableInteractionRelevanceAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_enableInteractionRelevanceAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( enableInteractionRelevanceAdvisorySwitchItem, (char *) this );


 // Creation of disableInteractionRelevanceAdvisorySwitchItem
 disableInteractionRelevanceAdvisorySwitchItem = XtVaCreateManagedWidget( "disableInteractionRelevanceAdvisorySwitchItem",
   xmPushButtonWidgetClass,
   advisorySwitchPane,
   RES_CONVERT( XmNlabelString, "Disable Interaction Relevance Advisory Switch..." ),
   NULL );
 XtAddCallback( disableInteractionRelevanceAdvisorySwitchItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_disableInteractionRelevanceAdvisorySwitchItem,
  (XtPointer) NULL );

 UxPutContext( disableInteractionRelevanceAdvisorySwitchItem, (char *) this );


 // Creation of advisorySwitchCMenu
 advisorySwitchCMenu = XtVaCreateManagedWidget( "advisorySwitchCMenu",
   xmCascadeButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Advisory Switch" ),
   XmNsubMenuId, advisorySwitchPane,
   NULL );
 UxPutContext( advisorySwitchCMenu, (char *) this );


 // Creation of supportMenu_b9
 supportMenu_b9 = XtVaCreateManagedWidget( "supportMenu_b9",
   xmSeparatorWidgetClass,
   supportMenu,
   NULL );
 UxPutContext( supportMenu_b9, (char *) this );


 // Creation of manualTickItem
 manualTickItem = XtVaCreateManagedWidget( "manualTickItem",
   xmPushButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Manual Tick..." ),
   NULL );
 XtAddCallback( manualTickItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_manualTickItem,
  (XtPointer) NULL );

 UxPutContext( manualTickItem, (char *) this );


 // Creation of autoTickItem
 autoTickItem = XtVaCreateManagedWidget( "autoTickItem",
   xmPushButtonWidgetClass,
   supportMenu,
   RES_CONVERT( XmNlabelString, "Enable Automatic Tick..." ),
   NULL );
 XtAddCallback( autoTickItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_autoTickItem,
  (XtPointer) NULL );

 UxPutContext( autoTickItem, (char *) this );


 // Creation of supportCMenu
 supportCMenu = XtVaCreateManagedWidget( "supportCMenu",
   xmCascadeButtonWidgetClass,
   menubar,
   RES_CONVERT( XmNlabelString, "Support" ),
   XmNsubMenuId, supportMenu,
   RES_CONVERT( XmNmnemonic, "S" ),
   NULL );
 UxPutContext( supportCMenu, (char *) this );


 // Creation of form1
 form1 = XtVaCreateManagedWidget( "form1",
   xmFormWidgetClass,
   mainWindow1,
   NULL );
 UxPutContext( form1, (char *) this );


 // Creation of statusLine
 statusLine = XtVaCreateManagedWidget( "statusLine",
   xmLabelWidgetClass,
   form1,
   RES_CONVERT( XmNlabelString, "" ),
   XmNrightAttachment, XmATTACH_FORM,
   XmNleftAttachment, XmATTACH_FORM,
   XmNtopAttachment, XmATTACH_NONE,
   XmNheight, 42,
   NULL );
 UxPutContext( statusLine, (char *) this );


 // Creation of separator4
 separator4 = XtVaCreateManagedWidget( "separator4",
   xmSeparatorWidgetClass,
   form1,
   XmNrightAttachment, XmATTACH_FORM,
   XmNleftAttachment, XmATTACH_FORM,
   XmNtopAttachment, XmATTACH_FORM,
   XmNtopOffset, 43,
   NULL );
 UxPutContext( separator4, (char *) this );


 // Creation of outputWindow
 outputWindow = XtVaCreateManagedWidget( "outputWindow",
   xmScrolledWindowWidgetClass,
   form1,
   XmNscrollingPolicy, XmAPPLICATION_DEFINED,
   XmNvisualPolicy, XmVARIABLE,
   XmNscrollBarDisplayPolicy, XmSTATIC,
   XmNbottomAttachment, XmATTACH_FORM,
   XmNrightAttachment, XmATTACH_FORM,
   XmNleftAttachment, XmATTACH_FORM,
   XmNtopAttachment, XmATTACH_WIDGET,
   XmNtopWidget, separator4,
   NULL );
 UxPutContext( outputWindow, (char *) this );


 // Creation of outputWindowText
 outputWindowText = XtVaCreateManagedWidget( "outputWindowText",
   xmTextWidgetClass,
   outputWindow,
   XmNwidth, 312,
   XmNheight, 107,
   XmNscrollHorizontal, FALSE,
   XmNwordWrap, TRUE,
   XmNcursorPositionVisible, FALSE,
   XmNeditMode, XmMULTI_LINE_EDIT ,
   XmNeditable, FALSE,
   NULL );
 UxPutContext( outputWindowText, (char *) this );


 // Creation of messageWindowMenu
 messageWindowMenu_shell = XtVaCreatePopupShell ("messageWindowMenu_shell",
   xmMenuShellWidgetClass, outputWindowText,
   XmNwidth, 1,
   XmNheight, 1,
   XmNallowShellResize, TRUE,
   XmNoverrideRedirect, TRUE,
   NULL );

 messageWindowMenu = XtVaCreateWidget( "messageWindowMenu",
   xmRowColumnWidgetClass,
   messageWindowMenu_shell,
   XmNrowColumnType, XmMENU_POPUP,
   XmNmenuAccelerator, "Shift <KeyUp>F10",
   NULL );
 UxPutContext( messageWindowMenu, (char *) this );


 // Creation of menu1_p1_title
 menu1_p1_title = XtVaCreateManagedWidget( "menu1_p1_title",
   xmLabelWidgetClass,
   messageWindowMenu,
   RES_CONVERT( XmNlabelString, "MESSAGES" ),
   NULL );
 UxPutContext( menu1_p1_title, (char *) this );


 // Creation of titleSeparator
 titleSeparator = XtVaCreateManagedWidget( "titleSeparator",
   xmSeparatorWidgetClass,
   messageWindowMenu,
   NULL );
 UxPutContext( titleSeparator, (char *) this );


 // Creation of clearWindowItem
 clearWindowItem = XtVaCreateManagedWidget( "clearWindowItem",
   xmPushButtonWidgetClass,
   messageWindowMenu,
   RES_CONVERT( XmNlabelString, "Clear Window" ),
   RES_CONVERT( XmNmnemonic, "C" ),
   NULL );
 XtAddCallback( clearWindowItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_clearWindowItem,
  (XtPointer) NULL );

 UxPutContext( clearWindowItem, (char *) this );


 // Creation of insertMarkerItem
 insertMarkerItem = XtVaCreateManagedWidget( "insertMarkerItem",
   xmPushButtonWidgetClass,
   messageWindowMenu,
   RES_CONVERT( XmNlabelString, "Insert Marker" ),
   RES_CONVERT( XmNmnemonic, "I" ),
   NULL );
 XtAddCallback( insertMarkerItem, XmNactivateCallback,
  (XtCallbackProc) &_UxCtestFederateShell::Wrap_activateCB_insertMarkerItem,
  (XtPointer) NULL );

 UxPutContext( insertMarkerItem, (char *) this );


 XtAddCallback( testFederateShell, XmNdestroyCallback,
  (XtCallbackProc) &_UxCtestFederateShell::UxDestroyContextCB,
  (XtPointer) this);

 XtAddEventHandler(outputWindowText, ButtonPressMask,
   False, (XtEventHandler) &_UxCtestFederateShell::MenuPost, (XtPointer) messageWindowMenu );
 XmMainWindowSetAreas( mainWindow1, menubar, (Widget) NULL,
   (Widget) NULL, (Widget) NULL, form1 );

 return ( testFederateShell );
}

/*******************************************************************************
       The following function includes the code that was entered
       in the 'Initial Code' and 'Final Code' sections of the
       Declarations Editor. This function is called from the
       'Interface function' below.
*******************************************************************************/

swidget _UxCtestFederateShell::_create_testFederateShell(void)
{
 Widget                  rtrn;

 // Initial code from declarations editor
 struct utsname  unameBuf;
 char            hostname[30];
 char            title[50];
 char           *timeInterval[] = {"100"};
 
 /* Initialize global variables */
 outputSelectionSwidget  = NULL;
 commandRecordingSwidget = NULL;
 replaySwidget           = NULL;
 
 commandFile       = NULL;
 strcpy (federateHandle, "");
 strcpy (federateName,   "");
 strcpy (federationName, "");
 outputDisplay     = True;
 outputRecording   = False;
 commandRecording  = False;
 commandReplay     = False;
 webBrowserVisible = False;
 webBrowserPID     = 0;
 autoTickEnabled   = False;
 
 signal (SIGCHLD, handleWebBrowserShutdownSignal);
 signal (SIGINT,  quit);
 signal (SIGTERM, quit);
 
 initializeHelpSystem ();
 UxThis = rtrn = _build();

 // Final Code from declarations editor
 if (uname (&unameBuf ) == -1)
   strcpy (hostname, "[]");
 else
   strcpy (hostname, unameBuf.nodename);
 
 sprintf (title, "%s - %s", applicationTitle, hostname);
 XtVaSetValues (UxGetWidget (testFederateShell),
         XmNtitle, title,
                NULL);
 
 updateStatus();
 
 //do_autoTick (timeInterval, TICK_NO_ARGS);
 
 return( rtrn );
}

/*******************************************************************************
       The following is the destructor function.
*******************************************************************************/

_UxCtestFederateShell::~_UxCtestFederateShell()
{
 if (this->UxThis)
 {
  XtRemoveCallback( UxGetWidget(this->UxThis),
   XmNdestroyCallback,
   (XtCallbackProc) &_UxCtestFederateShell::UxDestroyContextCB,
   (XtPointer) this);
  UxDestroyInterface(this->UxThis);
 }
}

/*******************************************************************************
       The following is the constructor function.
*******************************************************************************/

_UxCtestFederateShell::_UxCtestFederateShell( swidget UxParent )
{
 this->UxParent = UxParent;

 // User Supplied Constructor Code
}

/*******************************************************************************
       The following is the 'Interface function' which is the
       external entry point for creating this interface.
       This function should be called from your application or from
       a callback function.
*******************************************************************************/

swidget create_testFederateShell( swidget UxParent )
{
 _UxCtestFederateShell *theInterface = 
   new _UxCtestFederateShell( UxParent );
 return (theInterface->_create_testFederateShell());
}

/*******************************************************************************
       END OF FILE
*******************************************************************************/

