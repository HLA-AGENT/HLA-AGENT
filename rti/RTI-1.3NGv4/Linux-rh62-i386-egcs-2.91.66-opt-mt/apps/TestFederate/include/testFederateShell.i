! UIMX ascii 2.9 key: 6813                                                      

*testFederateShell.class: applicationShell
*testFederateShell.classinc:
*testFederateShell.classspec:
*testFederateShell.classmembers:
*testFederateShell.classconstructor:
*testFederateShell.classdestructor:
*testFederateShell.gbldecl: #include <stdio.h>\
#include <sys/types.h>\
#include <sys/utsname.h>\
#include <sys/wait.h>\
#include <unistd.h>\
#include <signal.h>\
#include <errno.h>\
\
#include "config.h"\
\
/* Creation functions for other interfaces */\
extern swidget create_outputSelectionShell (swidget);\
extern swidget create_commandRecordingShell (swidget);\
extern swidget create_replayShell (swidget);\
extern swidget create_genericPromptDialog (swidget, ServiceID);\
\
/* Swidgets that we will need global access to */\
swidget outputSelectionSwidget;\
swidget commandRecordingSwidget;\
swidget replaySwidget;\
\
/* Various other globals */\
FILE              *commandFile;\
FILE              *outputFile;       \
char               federateHandle[6];\
char               federateName[64];\
char               federationName[64];\
Boolean            outputDisplay;\
Boolean            outputRecording;\
Boolean            commandRecording;\
Boolean            commandReplay;\
Boolean            webBrowserVisible;\
pid_t              webBrowserPID;\
\
static Boolean  autoTickEnabled;\
static const char* const applicationTitle = "Test Federate v1.0";\
\
/* Other Function Prototypes */\
void           handleWebBrowserShutdownSignal (int siq);\
void           quit (int sig);\
void           updateStatus (void);\
void           outputMsg (char *message);\
void           initializeHelpSystem (void);\
void           autoTickEngaged (void);\
void           recordCommands (const ServiceID   serviceID,\
          const Boolean     extendedMode,\
          const int         promptCnt,\
          const char* const fields[]);\
extern void    do_autoTick (char **fields, int fieldCnt); \
#ifndef  DESIGN_TIME\
  extern void  generateRoundTripIteration (const int parmCnt = -1,\
                                           const int parmValueStringLength = -1); \
  extern void  generateRoundTripResponse ();\
  extern void  generateUpdateAttrIteration (const int attrCnt = -1,\
                                            const int attrValueStringLength = -1);\
#endif\
        \
static void    invokeUpdateStatus (XtPointer, XtIntervalId*);\
static void    invokeGenerateRoundTripIteration (XtPointer, XtIntervalId*);\
static void    invokeGenerateRoundTripResponse (XtPointer, XtIntervalId*);\
static void    invokeGenerateUpdateAttrIteration (XtPointer, XtIntervalId*);\
static void    stopCommandReplay (void);
*testFederateShell.ispecdecl:
*testFederateShell.funcdecl: swidget create_testFederateShell(swidget UxParent)
*testFederateShell.funcname: create_testFederateShell
*testFederateShell.funcdef: "swidget", "<create_testFederateShell>(%)"
*testFederateShell.argdecl: swidget UxParent;
*testFederateShell.arglist: UxParent
*testFederateShell.arglist.UxParent: "swidget", "%UxParent%"
*testFederateShell.icode: struct utsname  unameBuf;\
char            hostname[30];\
char            title[50];\
char           *timeInterval[] = {"100"};\
\
/* Initialize global variables */\
outputSelectionSwidget  = NULL;\
commandRecordingSwidget = NULL;\
replaySwidget           = NULL;\
\
commandFile       = NULL;\
strcpy (federateHandle, "");\
strcpy (federateName,   "");\
strcpy (federationName, "");\
outputDisplay     = True;\
outputRecording   = False;\
commandRecording  = False;\
commandReplay     = False;\
webBrowserVisible = False;\
webBrowserPID     = 0;\
autoTickEnabled   = False;\
\
signal (SIGCHLD, handleWebBrowserShutdownSignal);\
signal (SIGINT,  quit);\
signal (SIGTERM, quit);\
\
initializeHelpSystem ();
*testFederateShell.fcode: if (uname (&unameBuf ) == -1)\
  strcpy (hostname, "[]");\
else\
  strcpy (hostname, unameBuf.nodename);\
\
sprintf (title, "%s - %s", applicationTitle, hostname);\
XtVaSetValues (UxGetWidget (testFederateShell),\
        XmNtitle, title,\
               NULL);\
\
updateStatus();\
\
do_autoTick (timeInterval, TICK_NO_ARGS);\
\
return( rtrn );\

*testFederateShell.auxdecl: void handleWebBrowserShutdownSignal (int sig)\
{\
  int status;\
\
  if (wait (&status) == -1)\
  {\
    outputMsg ("Help Subsystem:  Wait Failure, probably due to a failure of Fork.");  \
    return;\
  }\
\
  webBrowserVisible = False;\
  webBrowserPID     = 0;\
\
  signal (SIGCHLD, handleWebBrowserShutdownSignal);\
}\
\
\
void quit (int sig)\
{\
  signal (SIGCHLD, SIG_IGN);\
\
  if (webBrowserVisible)\
  {\
    printf ("Shutting Down Web Browser...\n");\
    kill (webBrowserPID, SIGTERM);\
  }\
\
  exit (0);\
}\
\
\
void scheduleGenerateRoundTripIteration ()\
{\
  XtAppAddTimeOut (UxAppContext, 0, invokeGenerateRoundTripIteration, NULL);\
}\
\
static void invokeGenerateRoundTripIteration (XtPointer clientData, XtIntervalId *id)\
{\
#ifndef DESIGN_TIME\
  generateRoundTripIteration ();\
#endif\
}\
\
void scheduleGenerateRoundTripResponse ()\
{\
  XtAppAddTimeOut (UxAppContext, 0, invokeGenerateRoundTripResponse, NULL);\
}\
\
static void invokeGenerateRoundTripResponse (XtPointer clientData, XtIntervalId *id)\
{\
#ifndef DESIGN_TIME\
  generateRoundTripResponse ();\
#endif\
}\
\
\
void scheduleGenerateUpdateAttrIteration ()\
{\
  XtAppAddTimeOut (UxAppContext, 0, invokeGenerateUpdateAttrIteration, NULL);\
}\
\
static void invokeGenerateUpdateAttrIteration (XtPointer clientData, XtIntervalId *id)\
{\
#ifndef DESIGN_TIME\
  generateUpdateAttrIteration ();\
#endif\
}\
\
\
static void invokeUpdateStatus (XtPointer clientData, XtIntervalId *id)\
{\
  updateStatus ();\
}\
\
void updateStatus ()\
{\
  static Boolean  first = True;\
  char            statusString[250];\
  XmString        statusStr;\
\
  if (first)    /* This is a pure, unadulterated hack */\
  {\
    statusStr = XmStringCreateLtoR (" \n \n", XmFONTLIST_DEFAULT_TAG);\
    XtVaSetValues (UxGetWidget (statusLine),\
            XmNlabelString, statusStr,\
                   NULL);\
    XmStringFree (statusStr);\
\
    XtAppAddTimeOut (UxAppContext, 0, invokeUpdateStatus, NULL);\
\
    first = False;\
    return;\
  }\
\
   if (strcmp (federationName, "") != 0)\
    sprintf (statusString, "Federate: %s (%s)    Joined Federation: %s",\
             federateName, federateHandle, federationName);\
  else\
    sprintf (statusString, "Federate is Not Joined");\
\
  if (commandRecording)\
    strcat (statusString, "\nCommand Recording in Progress");\
  else if (commandReplay)\
    strcat (statusString, "\nCommand Replay in Progress");\
\
  if ((outputRecording) && (!outputDisplay))\
    strcat (statusString,\
            "\nOutput Recording in Progress    Output Display Disabled");\
  else if (outputRecording)\
    strcat (statusString, "\nOutput Recording in Progress");\
  else if (!outputDisplay)\
    strcat (statusString, "\nOutput Display Disabled");\
\
  statusStr = XmStringCreateLtoR (statusString, XmFONTLIST_DEFAULT_TAG);\
  XtVaSetValues (UxGetWidget (statusLine),\
          XmNlabelString, statusStr,\
                 NULL);\
  XmStringFree (statusStr);\
}\
\
\
void outputMsg (char *message)\
{\
  static Boolean  firstLine = True;\
  XmTextPosition  lastPosition;\
\
  if (outputDisplay)\
  {\
    if (!firstLine)\
    {\
      lastPosition = XmTextGetLastPosition (UxGetWidget (outputWindowText));\
      XmTextInsert (UxGetWidget (outputWindowText), lastPosition, "\n" );\
    }\
    else\
      firstLine = False;\
\
    lastPosition = XmTextGetLastPosition (UxGetWidget (outputWindowText));\
    XmTextInsert (UxGetWidget (outputWindowText), lastPosition, message);\
  }\
\
  if (outputRecording)\
    fprintf (outputFile, "%s\n", message); \
}\
\
\
void initializeHelpSystem (void)\
{\
#ifndef DESIGN_TIME\
  pid_t  pid;\
\
  if (!webBrowserVisible)\
  {\
    switch (pid = fork())\
    {\
      case -1:  /* Error during fork */\
        outputMsg ("Can't create new process.");\
        return;\
\
      case 0:   /* Child process */\
        execlp ("netscape",\
                "netscape", "-iconic", "IFSPEC10.html",\
                (char *)NULL);\
\
        printf ("Can't execute web browser.\n");\
        exit (-1);\
\
      default:  /* Parent process */\
        webBrowserVisible = True;\
        webBrowserPID     = pid;\
        printf ("Child process ID: %i\n", webBrowserPID);\
    }\
  }\
#endif\
}\
\
void autoTickEngaged (void)\
{\
  XmString autoTickItemLabel;\
\
  autoTickItemLabel = XmStringCreateLocalized ("Disable Automatic Tick");\
  XtVaSetValues (UxGetWidget (autoTickItem),\
                 XmNlabelString, autoTickItemLabel,\
                 NULL);\
  XmStringFree (autoTickItemLabel);\
\
  autoTickEnabled = True;\
}\
\
\
\
void recordCommands (const ServiceID   serviceID,\
       const Boolean     extendedMode,\
       const int         promptCnt,\
       const char* const fields[])\
{\
  int i;\
\
  if ((commandRecording) &&\
      (serviceID != TICK) && (serviceID != AUTO_TICK))\
  {\
    /* Output the command data to the file.  Note that there could be */\
    /* problems reading this record back in if the field strings      */\
    /* contain tab characters.                                        */\
    /* Empty fields are denoted by a '~'.                             */\
    fprintf (commandFile, "%i\t%i\t%i", serviceID, extendedMode, promptCnt);\
    for (i = 0; i < promptCnt; i++)\
      fprintf (commandFile, "\t%s",\
         (strcmp (fields[i], "") == 0) ? "~" : fields[i]);\
    fprintf (commandFile, "\n");\
  }\
}\
\
\
/* Read one line from the command file, parse it, execute the\
   requested service, then schedule the read of the next line.\
   Doing this one line at a time allows for federateAmbassador\
   calls to be serviced while playback is occurring.          */  \
void replayCommands (XtPointer clientData, XtIntervalId  *id)\
{\
#define MAX_COMMAND_LENGTH    500\
  char       command[MAX_COMMAND_LENGTH];\
  int        length;\
  char      *cmdArgument;\
  int        i;\
  ServiceID  serviceID;\
  int        extendedMode;\
  int        fieldCnt;\
  char     **fields;\
\
  if (commandFile != NULL)\
  {\
    if (fgets (command, MAX_COMMAND_LENGTH, commandFile) != NULL)\
    {\
      cmdArgument = strtok (command, "\t\n");\
      sscanf (cmdArgument, "%i", &serviceID);\
\
      cmdArgument = strtok (NULL, "\t\n");\
      sscanf (cmdArgument, "%i", &extendedMode);\
\
      cmdArgument = strtok (NULL, "\t\n");\
      sscanf (cmdArgument, "%i", &fieldCnt);\
\
      /* Use the fieldCnt above, and read in the fields */\
      fields = (char **)malloc (fieldCnt * sizeof (char *));\
      for (i = 0; i < fieldCnt; i++)\
      {\
        /* Empty fields are denoted by a '~'.  */\
 cmdArgument = strtok (NULL, "\t\n");\
        if (strcmp (cmdArgument, "~") == 0)\
        {\
          fields[i] = (char *)malloc (1);\
          strcpy (fields[i], "");\
        }\
        else\
        {\
          fields[i] = (char *)malloc (strlen (cmdArgument) + 1);\
          strcpy (fields[i], cmdArgument);\
        }\
      }\
\
      /* Now that we've constructed the fields array, call the service */\
      if (extendedMode)\
 promptRecs[serviceID].extendedForm->service (fields, fieldCnt);\
      else\
 promptRecs[serviceID].service (fields, fieldCnt);\
\
      /* Release the storage used by the fields array */\
      for (i = 0; i < fieldCnt; i++)\
 free (fields[i]);\
      free (fields);\
\
      /* Schedule the next command read. */\
      XtAppAddTimeOut (UxAppContext, 0, replayCommands, NULL);\
    }\
    else  /* End of File */\
    {\
      stopCommandReplay ();\
      return;\
    }\
  }\
}\
\
\
static void stopCommandReplay (void)\
{\
  XmString replayItemLabel;\
\
  fclose (commandFile);\
  commandFile   = NULL;\
  commandReplay = False;\
\
  replayItemLabel = XmStringCreateLocalized ("Replay File...");\
  XtVaSetValues (UxGetWidget (replayItem),\
                 XmNlabelString, replayItemLabel,\
                 NULL);\
  XmStringFree (replayItemLabel);\
\
\
  /* We are no longer in "Command Replay" mode, so get rid of the\
     status line that indicated that we were. */\
\
  updateStatus ();\
\
\
  /* Re-enable "Start Command Recording..." on the "File" menu. */\
\
  XtVaSetValues (UxGetWidget (recordItem),\
                 XmNsensitive, True,\
                 NULL);\
}
*testFederateShell.static: true
*testFederateShell.name: testFederateShell
*testFederateShell.parent: NO_PARENT
*testFederateShell.parentExpression: UxParent
*testFederateShell.x: 493
*testFederateShell.y: 409
*testFederateShell.width: 638
*testFederateShell.height: 284
*testFederateShell.iconName: "TestFed"
*testFederateShell.minHeight: 106
*testFederateShell.minWidth: 520

*mainWindow1.class: mainWindow
*mainWindow1.static: true
*mainWindow1.name: mainWindow1
*mainWindow1.parent: testFederateShell
*mainWindow1.unitType: "pixels"
*mainWindow1.x: 0
*mainWindow1.y: -1
*mainWindow1.width: 598
*mainWindow1.height: 168

*menubar.class: rowColumn
*menubar.static: true
*menubar.name: menubar
*menubar.parent: mainWindow1
*menubar.rowColumnType: "menu_bar"
*menubar.menuAccelerator: "<KeyUp>F10"

*fileMenu.class: rowColumn
*fileMenu.static: true
*fileMenu.name: fileMenu
*fileMenu.parent: menubar
*fileMenu.rowColumnType: "menu_pulldown"

*recordItem.class: pushButton
*recordItem.name.source: public
*recordItem.static: false
*recordItem.name: recordItem
*recordItem.parent: fileMenu
*recordItem.labelString: "Start Command Recording..."
*recordItem.mnemonic: "C"
*recordItem.activateCallback: XmString recordItemLabel;\
\
if (commandRecording)  /* Stop the current recording */\
{\
  fclose (commandFile);\
  commandFile      = NULL; \
  commandRecording = False;\
\
  recordItemLabel = XmStringCreateLocalized ("Start Command Recording...");\
  XtVaSetValues (UxGetWidget (recordItem),\
                 XmNlabelString, recordItemLabel,\
                 NULL);\
  XmStringFree (recordItemLabel);\
\
\
  /* We are no longer in "Command Record" mode, so get rid of the\
     status line that indicated that we were. */\
\
  updateStatus ();\
\
\
  /* Re-enable "File Replay" on the "File" menu. */\
\
  XtVaSetValues (UxGetWidget (replayItem),\
                 XmNsensitive, True,\
                 NULL);\
}\
\
else  /* Start a new recording.  Note that we don't actually start */\
      /* the recording until the user selects "Ok" in the Command  */\
{     /* Recording dialog box. */\
  if (commandRecordingSwidget == NULL)\
    commandRecordingSwidget = create_commandRecordingShell (NULL);\
\
  UxPopupInterface (commandRecordingSwidget, no_grab);\
}\
\


*replayItem.class: pushButton
*replayItem.name.source: public
*replayItem.static: false
*replayItem.name: replayItem
*replayItem.parent: fileMenu
*replayItem.labelString: "Replay File..."
*replayItem.mnemonic: "R"
*replayItem.activateCallback: if (commandReplay)  /* Stop the current replay */\
  stopCommandReplay (); \
else  /* Start a new replay.  Note that we don't actually start    */\
      /* the replay until the user selects "Ok" in the Command  */\
{     /* Replay dialog box. */\
  if (replaySwidget == NULL)\
    replaySwidget = create_replayShell (NULL);\
\
  UxPopupInterface (replaySwidget, no_grab);\
}\
\


*outputSelectionItem.class: pushButton
*outputSelectionItem.static: true
*outputSelectionItem.name: outputSelectionItem
*outputSelectionItem.parent: fileMenu
*outputSelectionItem.labelString: "Output Selection..."
*outputSelectionItem.mnemonic: "O"
*outputSelectionItem.activateCallback: if (outputSelectionSwidget == NULL)\
  outputSelectionSwidget = create_outputSelectionShell (NULL);\
UxPopupInterface (outputSelectionSwidget, no_grab);

*menu1_p1_b4.class: separator
*menu1_p1_b4.static: true
*menu1_p1_b4.name: menu1_p1_b4
*menu1_p1_b4.parent: fileMenu

*quitItem.class: pushButton
*quitItem.static: true
*quitItem.name: quitItem
*quitItem.parent: fileMenu
*quitItem.labelString: "Quit"
*quitItem.mnemonic: "Q"
*quitItem.activateCallback: quit( 0 );

*federationMenu.class: rowColumn
*federationMenu.static: true
*federationMenu.name: federationMenu
*federationMenu.parent: menubar
*federationMenu.rowColumnType: "menu_pulldown"

*createFedItem.class: pushButton
*createFedItem.static: true
*createFedItem.name: createFedItem
*createFedItem.parent: federationMenu
*createFedItem.labelString: "Create Federation Execution..."
*createFedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CREATE_FEDEX), no_grab);              \


*destroyFedItem.class: pushButton
*destroyFedItem.static: true
*destroyFedItem.name: destroyFedItem
*destroyFedItem.parent: federationMenu
*destroyFedItem.labelString: "Destroy Federation Execution..."
*destroyFedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DESTROY_FEDEX), no_grab);

*joinFedItem.class: pushButton
*joinFedItem.static: true
*joinFedItem.name: joinFedItem
*joinFedItem.parent: federationMenu
*joinFedItem.labelString: "Join Federation Execution..."
*joinFedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), JOIN_FEDEX), no_grab);

*resignItem.class: pushButton
*resignItem.static: true
*resignItem.name: resignItem
*resignItem.parent: federationMenu
*resignItem.labelString: "Resign Federation Execution..."
*resignItem.mnemonic: "R"
*resignItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RESIGN_FEDEX), no_grab);

*federationMenu_b17.class: separator
*federationMenu_b17.static: true
*federationMenu_b17.name: federationMenu_b17
*federationMenu_b17.parent: federationMenu

*registerSyncPointItem.class: pushButton
*registerSyncPointItem.static: true
*registerSyncPointItem.name: registerSyncPointItem
*registerSyncPointItem.parent: federationMenu
*registerSyncPointItem.labelString: "Register Federation Synchronization Point..."
*registerSyncPointItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REGISTER_SYNC_POINT), no_grab);

*syncPointAchievedItem.class: pushButton
*syncPointAchievedItem.static: true
*syncPointAchievedItem.name: syncPointAchievedItem
*syncPointAchievedItem.parent: federationMenu
*syncPointAchievedItem.labelString: "Synchronization Point Achieved..."
*syncPointAchievedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SYNC_POINT_ACHIEVED), no_grab);

*federationMenu_b18.class: separator
*federationMenu_b18.static: true
*federationMenu_b18.name: federationMenu_b18
*federationMenu_b18.parent: federationMenu

*requestFederationSaveItem.class: pushButton
*requestFederationSaveItem.static: true
*requestFederationSaveItem.name: requestFederationSaveItem
*requestFederationSaveItem.parent: federationMenu
*requestFederationSaveItem.labelString: "Request Federation Save..."
*requestFederationSaveItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_FEDERATION_SAVE), no_grab);

*federateSaveBegunItem.class: pushButton
*federateSaveBegunItem.static: true
*federateSaveBegunItem.name: federateSaveBegunItem
*federateSaveBegunItem.parent: federationMenu
*federateSaveBegunItem.labelString: "Federate Save Begun..."
*federateSaveBegunItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FEDERATE_SAVE_BEGUN), no_grab);

*federateSaveAchievedItem.class: pushButton
*federateSaveAchievedItem.static: true
*federateSaveAchievedItem.name: federateSaveAchievedItem
*federateSaveAchievedItem.parent: federationMenu
*federateSaveAchievedItem.labelString: "Federate Save Complete..."
*federateSaveAchievedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FEDERATE_SAVE_ACHIEVED), no_grab);              

*federateSaveNotAchievedItem.class: pushButton
*federateSaveNotAchievedItem.static: true
*federateSaveNotAchievedItem.name: federateSaveNotAchievedItem
*federateSaveNotAchievedItem.parent: federationMenu
*federateSaveNotAchievedItem.labelString: "Federate Save Not Complete..."
*federateSaveNotAchievedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FEDERATE_SAVE_NOT_ACHIEVED), no_grab);              

*requestRestoreItem.class: pushButton
*requestRestoreItem.static: true
*requestRestoreItem.name: requestRestoreItem
*requestRestoreItem.parent: federationMenu
*requestRestoreItem.labelString: "Request Federation Restore..."
*requestRestoreItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_RESTORE), no_grab);

*restoreAchievedItem.class: pushButton
*restoreAchievedItem.static: true
*restoreAchievedItem.name: restoreAchievedItem
*restoreAchievedItem.parent: federationMenu
*restoreAchievedItem.labelString: "Federate Restore Complete..."
*restoreAchievedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RESTORE_ACHIEVED), no_grab);

*restoreNotAchievedItem.class: pushButton
*restoreNotAchievedItem.static: true
*restoreNotAchievedItem.name: restoreNotAchievedItem
*restoreNotAchievedItem.parent: federationMenu
*restoreNotAchievedItem.labelString: "Federate Restore Not Complete..."
*restoreNotAchievedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RESTORE_NOT_ACHIEVED), no_grab);

*declarationMenu.class: rowColumn
*declarationMenu.static: true
*declarationMenu.name: declarationMenu
*declarationMenu.parent: menubar
*declarationMenu.rowColumnType: "menu_pulldown"

*publicationsCMenu.class: cascadeButton
*publicationsCMenu.static: true
*publicationsCMenu.name: publicationsCMenu
*publicationsCMenu.parent: declarationMenu
*publicationsCMenu.labelString: "Publications"
*publicationsCMenu.subMenuId: "publicationsPane"
*publicationsCMenu.mnemonic: "P"

*subscriptionsCMenu.class: cascadeButton
*subscriptionsCMenu.static: true
*subscriptionsCMenu.name: subscriptionsCMenu
*subscriptionsCMenu.parent: declarationMenu
*subscriptionsCMenu.labelString: "Subscriptions"
*subscriptionsCMenu.mnemonic: "S"
*subscriptionsCMenu.subMenuId: "subscriptionsPane"

*publicationsPane.class: rowColumn
*publicationsPane.static: true
*publicationsPane.name: publicationsPane
*publicationsPane.parent: declarationMenu
*publicationsPane.rowColumnType: "menu_pulldown"

*publishObjItem.class: pushButton
*publishObjItem.static: true
*publishObjItem.name: publishObjItem
*publishObjItem.parent: publicationsPane
*publishObjItem.labelString: "Publish Object Class..."
*publishObjItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), PUBLISH_OBJ_CLASS), no_grab);

*unpublishObjItem.class: pushButton
*unpublishObjItem.static: true
*unpublishObjItem.name: unpublishObjItem
*unpublishObjItem.parent: publicationsPane
*unpublishObjItem.labelString: "Unpublish Object Class..."
*unpublishObjItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNPUBLISH_OBJ_CLASS), no_grab);

*publishInteractionItem.class: pushButton
*publishInteractionItem.static: true
*publishInteractionItem.name: publishInteractionItem
*publishInteractionItem.parent: publicationsPane
*publishInteractionItem.labelString: "Publish Interaction Class..."
*publishInteractionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), PUBLISH_INTERACTION_CLASS), no_grab);

*unpublishInteractionItem.class: pushButton
*unpublishInteractionItem.static: true
*unpublishInteractionItem.name: unpublishInteractionItem
*unpublishInteractionItem.parent: publicationsPane
*unpublishInteractionItem.labelString: "Unpublish Interaction Class..."
*unpublishInteractionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNPUBLISH_INTERACTION_CLASS), no_grab);

*subscriptionsPane.class: rowColumn
*subscriptionsPane.static: true
*subscriptionsPane.name: subscriptionsPane
*subscriptionsPane.parent: declarationMenu
*subscriptionsPane.rowColumnType: "menu_pulldown"

*subscribeObjectClassItem.class: pushButton
*subscribeObjectClassItem.static: true
*subscribeObjectClassItem.name: subscribeObjectClassItem
*subscribeObjectClassItem.parent: subscriptionsPane
*subscribeObjectClassItem.labelString: "Subscribe Object Class Attributes..."
*subscribeObjectClassItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_OBJ_CLASS_ATTR), no_grab);

*unsubscribeObjectClassItem.class: pushButton
*unsubscribeObjectClassItem.static: true
*unsubscribeObjectClassItem.name: unsubscribeObjectClassItem
*unsubscribeObjectClassItem.parent: subscriptionsPane
*unsubscribeObjectClassItem.labelString: "Unsubscribe Object Class..."
*unsubscribeObjectClassItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_OBJ_CLASS_ATTR), no_grab);

*subscribeIntItem.class: pushButton
*subscribeIntItem.static: true
*subscribeIntItem.name: subscribeIntItem
*subscribeIntItem.parent: subscriptionsPane
*subscribeIntItem.labelString: "Subscribe Interaction Class..."
*subscribeIntItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_INTERACTION_CLASS), no_grab);

*unsubscribeIntItem.class: pushButton
*unsubscribeIntItem.static: true
*unsubscribeIntItem.name: unsubscribeIntItem
*unsubscribeIntItem.parent: subscriptionsPane
*unsubscribeIntItem.labelString: "Unsubscribe Interaction Class..."
*unsubscribeIntItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_INTERACTION_CLASS), no_grab);

*objectMenu.class: rowColumn
*objectMenu.name.source: public
*objectMenu.static: false
*objectMenu.name: objectMenu
*objectMenu.parent: menubar
*objectMenu.rowColumnType: "menu_pulldown"

*registerObjectItem.class: pushButton
*registerObjectItem.static: true
*registerObjectItem.name: registerObjectItem
*registerObjectItem.parent: objectMenu
*registerObjectItem.labelString: "Register Object Instance..."
*registerObjectItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REGISTER_OBJ), no_grab);\


*updateAttrValuesItem.class: pushButton
*updateAttrValuesItem.static: true
*updateAttrValuesItem.name: updateAttrValuesItem
*updateAttrValuesItem.parent: objectMenu
*updateAttrValuesItem.labelString: "Update Attribute Values..."
*updateAttrValuesItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UPDATE_ATTR_VALUES), no_grab);\


*sendInteractionItem.class: pushButton
*sendInteractionItem.static: true
*sendInteractionItem.name: sendInteractionItem
*sendInteractionItem.parent: objectMenu
*sendInteractionItem.labelString: "Send Interaction..."
*sendInteractionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SEND_INTERACTION), no_grab);

*deleteObjectItem.class: pushButton
*deleteObjectItem.static: true
*deleteObjectItem.name: deleteObjectItem
*deleteObjectItem.parent: objectMenu
*deleteObjectItem.labelString: "Delete Object Instance..."
*deleteObjectItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DELETE_OBJ), no_grab);

*localDeleteObjectItem.class: pushButton
*localDeleteObjectItem.static: true
*localDeleteObjectItem.name: localDeleteObjectItem
*localDeleteObjectItem.parent: objectMenu
*localDeleteObjectItem.labelString: "Local Delete Object Instance..."
*localDeleteObjectItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), LOCAL_DELETE_OBJ), no_grab);

*objectsMenu_b18.class: separator
*objectsMenu_b18.static: true
*objectsMenu_b18.name: objectsMenu_b18
*objectsMenu_b18.parent: objectMenu

*attrTransportTypeItem.class: pushButton
*attrTransportTypeItem.static: true
*attrTransportTypeItem.name: attrTransportTypeItem
*attrTransportTypeItem.parent: objectMenu
*attrTransportTypeItem.labelString: "Change Attribute Transport Type..."
*attrTransportTypeItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_ATTR_TRANSPORT_TYPE), no_grab);

*interactionTransportTypeItem.class: pushButton
*interactionTransportTypeItem.static: true
*interactionTransportTypeItem.name: interactionTransportTypeItem
*interactionTransportTypeItem.parent: objectMenu
*interactionTransportTypeItem.labelString: "Change Interaction Transport Type..."
*interactionTransportTypeItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_INTERACTION_TRANSPORT_TYPE), no_grab);

*requestObjectUpdateItem.class: pushButton
*requestObjectUpdateItem.static: true
*requestObjectUpdateItem.name: requestObjectUpdateItem
*requestObjectUpdateItem.parent: objectMenu
*requestObjectUpdateItem.labelString: "Request Object Attribute Value Update..."
*requestObjectUpdateItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_OBJ_ATTR_VALUE_UPDATE), no_grab);

*requestClassUpdateItem.class: pushButton
*requestClassUpdateItem.static: true
*requestClassUpdateItem.name: requestClassUpdateItem
*requestClassUpdateItem.parent: objectMenu
*requestClassUpdateItem.labelString: "Request Class Attribute Value Update..."
*requestClassUpdateItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_CLASS_ATTR_VALUE_UPDATE), no_grab);

*ownershipMenu.class: rowColumn
*ownershipMenu.name.source: public
*ownershipMenu.static: false
*ownershipMenu.name: ownershipMenu
*ownershipMenu.parent: menubar
*ownershipMenu.rowColumnType: "menu_pulldown"

*uncondAttrOwnDivItem.class: pushButton
*uncondAttrOwnDivItem.static: true
*uncondAttrOwnDivItem.name: uncondAttrOwnDivItem
*uncondAttrOwnDivItem.parent: ownershipMenu
*uncondAttrOwnDivItem.labelString: "Unconditional Attribute Ownership Divestiture..."
*uncondAttrOwnDivItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNCONDITIONAL_ATTR_OWNERSHIP_DIVESTITURE), no_grab); 

*negAttrOwnDivItem.class: pushButton
*negAttrOwnDivItem.static: true
*negAttrOwnDivItem.name: negAttrOwnDivItem
*negAttrOwnDivItem.parent: ownershipMenu
*negAttrOwnDivItem.labelString: "Negotiated Attribute Ownership Divestiture..."
*negAttrOwnDivItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NEGOTIATED_ATTR_OWNERSHIP_DIVESTITURE), no_grab); 

*attrOwnAcqItem.class: pushButton
*attrOwnAcqItem.static: true
*attrOwnAcqItem.name: attrOwnAcqItem
*attrOwnAcqItem.parent: ownershipMenu
*attrOwnAcqItem.labelString: "Attribute Ownership Acquisition..."
*attrOwnAcqItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_OWNERSHIP_ACQUISITION), no_grab); 

*attrOwnAcqIfAvaItem.class: pushButton
*attrOwnAcqIfAvaItem.static: true
*attrOwnAcqIfAvaItem.name: attrOwnAcqIfAvaItem
*attrOwnAcqIfAvaItem.parent: ownershipMenu
*attrOwnAcqIfAvaItem.labelString: "Attribute Ownership Acquisition If Available..."
*attrOwnAcqIfAvaItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_OWNERSHIP_ACQUISITION_IF_AVAILABLE), no_grab); 

*attrOwnRelResponseItem.class: pushButton
*attrOwnRelResponseItem.static: true
*attrOwnRelResponseItem.name: attrOwnRelResponseItem
*attrOwnRelResponseItem.parent: ownershipMenu
*attrOwnRelResponseItem.labelString: "Attribute Ownership Release Response..."
*attrOwnRelResponseItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_OWNERSHIP_RELEASE_RESPONSE), no_grab); 

*cancelOwnDivItem.class: pushButton
*cancelOwnDivItem.static: true
*cancelOwnDivItem.name: cancelOwnDivItem
*cancelOwnDivItem.parent: ownershipMenu
*cancelOwnDivItem.labelString: "Cancel Negotiated Attribute Ownership Divestiture..."
*cancelOwnDivItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CANCEL_NEGOTIATED_ATTR_OWNERSHIP_DIVESTITURE), no_grab); 

*cancelOwnAcqItem.class: pushButton
*cancelOwnAcqItem.static: true
*cancelOwnAcqItem.name: cancelOwnAcqItem
*cancelOwnAcqItem.parent: ownershipMenu
*cancelOwnAcqItem.labelString: "Cancel Attribute Ownership Acquisition..."
*cancelOwnAcqItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CANCEL_ATTR_OWNERSHIP_ACQUISITION), no_grab); 

*queryAttrOwnItem.class: pushButton
*queryAttrOwnItem.static: true
*queryAttrOwnItem.name: queryAttrOwnItem
*queryAttrOwnItem.parent: ownershipMenu
*queryAttrOwnItem.labelString: "Query Attribute Ownership..."
*queryAttrOwnItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), QUERY_ATTR_OWNERSHIP), no_grab); 

*attrIsOwnedByFederateItem.class: pushButton
*attrIsOwnedByFederateItem.static: true
*attrIsOwnedByFederateItem.name: attrIsOwnedByFederateItem
*attrIsOwnedByFederateItem.parent: ownershipMenu
*attrIsOwnedByFederateItem.labelString: "Is Attribute Owned By Federate..."
*attrIsOwnedByFederateItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ATTR_IS_OWNED_BY_FEDERATE), no_grab);

*timeMenu.class: rowColumn
*timeMenu.static: true
*timeMenu.name: timeMenu
*timeMenu.parent: menubar
*timeMenu.rowColumnType: "menu_pulldown"

*enableTimeRegulationItem.class: pushButton
*enableTimeRegulationItem.static: true
*enableTimeRegulationItem.name: enableTimeRegulationItem
*enableTimeRegulationItem.parent: timeMenu
*enableTimeRegulationItem.labelString: "Enable Time Regulation ..."
*enableTimeRegulationItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_TIME_REGULATION), no_grab);

*disableTimeRegulationItem.class: pushButton
*disableTimeRegulationItem.static: true
*disableTimeRegulationItem.name: disableTimeRegulationItem
*disableTimeRegulationItem.parent: timeMenu
*disableTimeRegulationItem.labelString: "Disable Time Regulation ..."
*disableTimeRegulationItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_TIME_REGULATION), no_grab);

*enableTimeConstrainedItem.class: pushButton
*enableTimeConstrainedItem.static: true
*enableTimeConstrainedItem.name: enableTimeConstrainedItem
*enableTimeConstrainedItem.parent: timeMenu
*enableTimeConstrainedItem.labelString: "Enable Time Constrained ..."
*enableTimeConstrainedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_TIME_CONSTRAINED), no_grab);

*disableTimeConstrainedItem.class: pushButton
*disableTimeConstrainedItem.static: true
*disableTimeConstrainedItem.name: disableTimeConstrainedItem
*disableTimeConstrainedItem.parent: timeMenu
*disableTimeConstrainedItem.labelString: "Disable Time Constrained ..."
*disableTimeConstrainedItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_TIME_CONSTRAINED), no_grab);

*timeMenu_b17.class: separator
*timeMenu_b17.static: true
*timeMenu_b17.name: timeMenu_b17
*timeMenu_b17.parent: timeMenu

*timeAdvanceReqItem.class: pushButton
*timeAdvanceReqItem.static: true
*timeAdvanceReqItem.name: timeAdvanceReqItem
*timeAdvanceReqItem.parent: timeMenu
*timeAdvanceReqItem.labelString: "Time Advance Request..."
*timeAdvanceReqItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), TIME_ADVANCE_REQUEST), no_grab);

*timeAdvanceReqAvailableItem.class: pushButton
*timeAdvanceReqAvailableItem.static: true
*timeAdvanceReqAvailableItem.name: timeAdvanceReqAvailableItem
*timeAdvanceReqAvailableItem.parent: timeMenu
*timeAdvanceReqAvailableItem.labelString: "Time Advance Request Available..."
*timeAdvanceReqAvailableItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), TIME_ADVANCE_REQUEST_AVAILABLE), no_grab);\


*nextEventReqItem.class: pushButton
*nextEventReqItem.static: true
*nextEventReqItem.name: nextEventReqItem
*nextEventReqItem.parent: timeMenu
*nextEventReqItem.labelString: "Next Event Request..."
*nextEventReqItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NEXT_EVENT_REQUEST), no_grab);

*nextEventReqAvailableItem.class: pushButton
*nextEventReqAvailableItem.static: true
*nextEventReqAvailableItem.name: nextEventReqAvailableItem
*nextEventReqAvailableItem.parent: timeMenu
*nextEventReqAvailableItem.labelString: "Next Event Request Available..."
*nextEventReqAvailableItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NEXT_EVENT_REQUEST_AVAILABLE), no_grab);\


*flushQueueRequestsItem.class: pushButton
*flushQueueRequestsItem.static: true
*flushQueueRequestsItem.name: flushQueueRequestsItem
*flushQueueRequestsItem.parent: timeMenu
*flushQueueRequestsItem.labelString: "Flush Queue Request..."
*flushQueueRequestsItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), FLUSH_QUEUE_REQUEST), no_grab);

*timeMenu_b8.class: separator
*timeMenu_b8.static: true
*timeMenu_b8.name: timeMenu_b8
*timeMenu_b8.parent: timeMenu

*enableAsynchronousDeliveryItem.class: pushButton
*enableAsynchronousDeliveryItem.static: true
*enableAsynchronousDeliveryItem.name: enableAsynchronousDeliveryItem
*enableAsynchronousDeliveryItem.parent: timeMenu
*enableAsynchronousDeliveryItem.labelString: "Enable Asynchronous Delivery..."
*enableAsynchronousDeliveryItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_ASYNCHRONOUS_DELIVERY), no_grab);

*disableAsynchronousDeliveryItem.class: pushButton
*disableAsynchronousDeliveryItem.static: true
*disableAsynchronousDeliveryItem.name: disableAsynchronousDeliveryItem
*disableAsynchronousDeliveryItem.parent: timeMenu
*disableAsynchronousDeliveryItem.labelString: "Disable Asynchronous Delivery..."
*disableAsynchronousDeliveryItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_ASYNCHRONOUS_DELIVERY), no_grab);

*timeMenu_b18.class: separator
*timeMenu_b18.static: true
*timeMenu_b18.name: timeMenu_b18
*timeMenu_b18.parent: timeMenu

*requestLBTSitem.class: pushButton
*requestLBTSitem.static: true
*requestLBTSitem.name: requestLBTSitem
*requestLBTSitem.parent: timeMenu
*requestLBTSitem.labelString: "Query LBTS..."
*requestLBTSitem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_LBTS), no_grab);

*requestFederateTimeItem.class: pushButton
*requestFederateTimeItem.static: true
*requestFederateTimeItem.name: requestFederateTimeItem
*requestFederateTimeItem.parent: timeMenu
*requestFederateTimeItem.labelString: "Query Federate Time..."
*requestFederateTimeItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_FEDERATE_TIME), no_grab);

*requestMinEventTimeItem.class: pushButton
*requestMinEventTimeItem.static: true
*requestMinEventTimeItem.name: requestMinEventTimeItem
*requestMinEventTimeItem.parent: timeMenu
*requestMinEventTimeItem.labelString: "Query Minimum Next Event Time..."
*requestMinEventTimeItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_MIN_NEXT_EVENT_TIME), no_grab);

*changeLookaheadItem.class: pushButton
*changeLookaheadItem.static: true
*changeLookaheadItem.name: changeLookaheadItem
*changeLookaheadItem.parent: timeMenu
*changeLookaheadItem.labelString: "Modify Lookahead..."
*changeLookaheadItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_LOOKAHEAD), no_grab);

*requestLookaheadItem.class: pushButton
*requestLookaheadItem.static: true
*requestLookaheadItem.name: requestLookaheadItem
*requestLookaheadItem.parent: timeMenu
*requestLookaheadItem.labelString: "Query Lookahead..."
*requestLookaheadItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_LOOKAHEAD), no_grab);

*timeMenu_b20.class: separator
*timeMenu_b20.static: true
*timeMenu_b20.name: timeMenu_b20
*timeMenu_b20.parent: timeMenu

*retractItem.class: pushButton
*retractItem.static: true
*retractItem.name: retractItem
*retractItem.parent: timeMenu
*retractItem.labelString: "Retract..."
*retractItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), RETRACT), no_grab);

*changeAttrOrderTypeItem.class: pushButton
*changeAttrOrderTypeItem.static: true
*changeAttrOrderTypeItem.name: changeAttrOrderTypeItem
*changeAttrOrderTypeItem.parent: timeMenu
*changeAttrOrderTypeItem.labelString: "Change Attribute Order Type..."
*changeAttrOrderTypeItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_ATTR_ORDER_TYPE), no_grab);

*changeInteractionOrderTypeItem.class: pushButton
*changeInteractionOrderTypeItem.static: true
*changeInteractionOrderTypeItem.name: changeInteractionOrderTypeItem
*changeInteractionOrderTypeItem.parent: timeMenu
*changeInteractionOrderTypeItem.labelString: "Change Interaction Order Type..."
*changeInteractionOrderTypeItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CHANGE_INTERACTION_ORDER_TYPE), no_grab);

*DDMmenu.class: rowColumn
*DDMmenu.static: true
*DDMmenu.name: DDMmenu
*DDMmenu.parent: menubar
*DDMmenu.rowColumnType: "menu_pulldown"

*createUpdateRegionItem.class: pushButton
*createUpdateRegionItem.static: true
*createUpdateRegionItem.name: createUpdateRegionItem
*createUpdateRegionItem.parent: DDMmenu
*createUpdateRegionItem.labelString: "Create Region ..."
*createUpdateRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), CREATE_REGION), no_grab);

*modifyRegionItem.class: pushButton
*modifyRegionItem.static: true
*modifyRegionItem.name: modifyRegionItem
*modifyRegionItem.parent: DDMmenu
*modifyRegionItem.labelString: "Modify Region ..."
*modifyRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), NOTIFY_REGION_MODIFICATION), no_grab);

*deleteRegionItem.class: pushButton
*deleteRegionItem.static: true
*deleteRegionItem.name: deleteRegionItem
*deleteRegionItem.parent: DDMmenu
*deleteRegionItem.labelString: "Delete Region ..."
*deleteRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DELETE_REGION), no_grab);

*registerObjWIthRegionItem.class: pushButton
*registerObjWIthRegionItem.static: true
*registerObjWIthRegionItem.name: registerObjWIthRegionItem
*registerObjWIthRegionItem.parent: DDMmenu
*registerObjWIthRegionItem.labelString: "Register Object Instance With Region ..."
*registerObjWIthRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REGISTER_OBJ_WITH_REGION), no_grab);

*DDMmenu_b7.class: separator
*DDMmenu_b7.static: true
*DDMmenu_b7.name: DDMmenu_b7
*DDMmenu_b7.parent: DDMmenu

*regionAssociationCMenu.class: cascadeButton
*regionAssociationCMenu.static: true
*regionAssociationCMenu.name: regionAssociationCMenu
*regionAssociationCMenu.parent: DDMmenu
*regionAssociationCMenu.labelString: "Associations"
*regionAssociationCMenu.subMenuId: "regionAssociationPane"

*subscriptionWithRegionCMenu.class: cascadeButton
*subscriptionWithRegionCMenu.static: true
*subscriptionWithRegionCMenu.name: subscriptionWithRegionCMenu
*subscriptionWithRegionCMenu.parent: DDMmenu
*subscriptionWithRegionCMenu.labelString: "Subscription With Region"
*subscriptionWithRegionCMenu.subMenuId: "subscriptionWithRegionPane"

*DDMmenu_b11.class: separator
*DDMmenu_b11.static: true
*DDMmenu_b11.name: DDMmenu_b11
*DDMmenu_b11.parent: DDMmenu

*sendInteractionWithRegionItem.class: pushButton
*sendInteractionWithRegionItem.static: true
*sendInteractionWithRegionItem.name: sendInteractionWithRegionItem
*sendInteractionWithRegionItem.parent: DDMmenu
*sendInteractionWithRegionItem.labelString: "Send Interaction With Region ..."
*sendInteractionWithRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SEND_INTERACTION_WITH_REGION), no_grab);

*reqClassAttrValueUpdateWithRegionItem.class: pushButton
*reqClassAttrValueUpdateWithRegionItem.static: true
*reqClassAttrValueUpdateWithRegionItem.name: reqClassAttrValueUpdateWithRegionItem
*reqClassAttrValueUpdateWithRegionItem.parent: DDMmenu
*reqClassAttrValueUpdateWithRegionItem.labelString: "Request Attribute Value Update With Region ..."
*reqClassAttrValueUpdateWithRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), REQUEST_CLASS_ATTRIBUTE_VALUE_UPDATE_WITH_REGION), no_grab);

*DDMmenu_b12.class: separator
*DDMmenu_b12.static: true
*DDMmenu_b12.name: DDMmenu_b12
*DDMmenu_b12.parent: DDMmenu

*aboutRegionCMenu.class: cascadeButton
*aboutRegionCMenu.static: true
*aboutRegionCMenu.name: aboutRegionCMenu
*aboutRegionCMenu.parent: DDMmenu
*aboutRegionCMenu.labelString: "About Region"
*aboutRegionCMenu.subMenuId: "aboutRegionPane"

*regionAssociationPane.class: rowColumn
*regionAssociationPane.static: true
*regionAssociationPane.name: regionAssociationPane
*regionAssociationPane.parent: DDMmenu
*regionAssociationPane.rowColumnType: "menu_pulldown"

*associateRegionForUpdatesItem.class: pushButton
*associateRegionForUpdatesItem.static: true
*associateRegionForUpdatesItem.name: associateRegionForUpdatesItem
*associateRegionForUpdatesItem.parent: regionAssociationPane
*associateRegionForUpdatesItem.labelString: "Associate Region for Updates ..."
*associateRegionForUpdatesItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ASSOCIATE_REGION_FOR_UPDATES), no_grab);

*unassociateRegionForUPdatesItem.class: pushButton
*unassociateRegionForUPdatesItem.static: true
*unassociateRegionForUPdatesItem.name: unassociateRegionForUPdatesItem
*unassociateRegionForUPdatesItem.parent: regionAssociationPane
*unassociateRegionForUPdatesItem.labelString: "Unassociate Region For Updates ..."
*unassociateRegionForUPdatesItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNASSOCIATE_REGION_FOR_UPDATES), no_grab);

*subscriptionWithRegionPane.class: rowColumn
*subscriptionWithRegionPane.static: true
*subscriptionWithRegionPane.name: subscriptionWithRegionPane
*subscriptionWithRegionPane.parent: DDMmenu
*subscriptionWithRegionPane.rowColumnType: "menu_pulldown"

*subObjAttrWithRegionItem.class: pushButton
*subObjAttrWithRegionItem.static: true
*subObjAttrWithRegionItem.name: subObjAttrWithRegionItem
*subObjAttrWithRegionItem.parent: subscriptionWithRegionPane
*subObjAttrWithRegionItem.labelString: "Subscribe Object Class Attributes With Region ..."
*subObjAttrWithRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_OBJ_CLASS_ATTR_WITH_REGION), no_grab);

*unsubObjClassWithRegionItem.class: pushButton
*unsubObjClassWithRegionItem.static: true
*unsubObjClassWithRegionItem.name: unsubObjClassWithRegionItem
*unsubObjClassWithRegionItem.parent: subscriptionWithRegionPane
*unsubObjClassWithRegionItem.labelString: "Unsubscribe Object Class With Region ..."
*unsubObjClassWithRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_OBJ_CLASS_WITH_REGION), no_grab);

*subIntClassWithRegionItem.class: pushButton
*subIntClassWithRegionItem.static: true
*subIntClassWithRegionItem.name: subIntClassWithRegionItem
*subIntClassWithRegionItem.parent: subscriptionWithRegionPane
*subIntClassWithRegionItem.labelString: "Subscribe Interaction Class With Region ..."
*subIntClassWithRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SUBSCRIBE_INTERACTION_CLASS_WITH_REGION), no_grab);

*unsubIntClassWithRegion.class: pushButton
*unsubIntClassWithRegion.static: true
*unsubIntClassWithRegion.name: unsubIntClassWithRegion
*unsubIntClassWithRegion.parent: subscriptionWithRegionPane
*unsubIntClassWithRegion.labelString: "Unsubscribe Interaction Class With Region ..."
*unsubIntClassWithRegion.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), UNSUBSCRIBE_INTERACTION_CLASS_WITH_REGION), no_grab);

*aboutRegionPane.class: rowColumn
*aboutRegionPane.static: true
*aboutRegionPane.name: aboutRegionPane
*aboutRegionPane.parent: DDMmenu
*aboutRegionPane.rowColumnType: "menu_pulldown"

*setRangeLowerBoundItem.class: pushButton
*setRangeLowerBoundItem.static: true
*setRangeLowerBoundItem.name: setRangeLowerBoundItem
*setRangeLowerBoundItem.parent: aboutRegionPane
*setRangeLowerBoundItem.labelString: "Set Range Lower Bound..."
*setRangeLowerBoundItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SET_RANGE_LOWER_BOUND), no_grab);

*setRangeUpperBoundItem.class: pushButton
*setRangeUpperBoundItem.static: true
*setRangeUpperBoundItem.name: setRangeUpperBoundItem
*setRangeUpperBoundItem.parent: aboutRegionPane
*setRangeUpperBoundItem.labelString: "Set Range Upper Bound..."
*setRangeUpperBoundItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), SET_RANGE_UPPER_BOUND), no_grab);

*getRangeLowerBoundItem.class: pushButton
*getRangeLowerBoundItem.static: true
*getRangeLowerBoundItem.name: getRangeLowerBoundItem
*getRangeLowerBoundItem.parent: aboutRegionPane
*getRangeLowerBoundItem.labelString: "Get Range Lower Bound..."
*getRangeLowerBoundItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_LOWER_BOUND), no_grab);

*getRangeUpperBoundItem.class: pushButton
*getRangeUpperBoundItem.static: true
*getRangeUpperBoundItem.name: getRangeUpperBoundItem
*getRangeUpperBoundItem.parent: aboutRegionPane
*getRangeUpperBoundItem.labelString: "Get Range Upper Bound..."
*getRangeUpperBoundItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_UPPER_BOUND), no_grab);

*getSpaceHandleForRegionItem.class: pushButton
*getSpaceHandleForRegionItem.static: true
*getSpaceHandleForRegionItem.name: getSpaceHandleForRegionItem
*getSpaceHandleForRegionItem.parent: aboutRegionPane
*getSpaceHandleForRegionItem.labelString: "Get Space Handle For Region..."
*getSpaceHandleForRegionItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_SPACE_HANDLE_FOR_REGION), no_grab);

*getNumberOfExtentsItem.class: pushButton
*getNumberOfExtentsItem.static: true
*getNumberOfExtentsItem.name: getNumberOfExtentsItem
*getNumberOfExtentsItem.parent: aboutRegionPane
*getNumberOfExtentsItem.labelString: "Get Number Of Extents..."
*getNumberOfExtentsItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_NUMBER_OF_EXTENTS), no_grab);

*getRangeLowerBoundNotificationLimitItem.class: pushButton
*getRangeLowerBoundNotificationLimitItem.static: true
*getRangeLowerBoundNotificationLimitItem.name: getRangeLowerBoundNotificationLimitItem
*getRangeLowerBoundNotificationLimitItem.parent: aboutRegionPane
*getRangeLowerBoundNotificationLimitItem.labelString: "Get Range Lower Bound Notification Limit..."
*getRangeLowerBoundNotificationLimitItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_LOWER_BOUND_NOTIFICATION_LIMIT), no_grab);

*getRangeUpperBoundNotificationLimitItem.class: pushButton
*getRangeUpperBoundNotificationLimitItem.static: true
*getRangeUpperBoundNotificationLimitItem.name: getRangeUpperBoundNotificationLimitItem
*getRangeUpperBoundNotificationLimitItem.parent: aboutRegionPane
*getRangeUpperBoundNotificationLimitItem.labelString: "Get Range Upper Bound Notification Limit..."
*getRangeUpperBoundNotificationLimitItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_RANGE_UPPER_BOUND_NOTIFICATION_LIMIT), no_grab);

*supportMenu.class: rowColumn
*supportMenu.static: true
*supportMenu.name: supportMenu
*supportMenu.parent: menubar
*supportMenu.rowColumnType: "menu_pulldown"

*nameToHandleCMenu.class: cascadeButton
*nameToHandleCMenu.static: true
*nameToHandleCMenu.name: nameToHandleCMenu
*nameToHandleCMenu.parent: supportMenu
*nameToHandleCMenu.labelString: "Name to Handle"
*nameToHandleCMenu.subMenuId: "nameToHandlePane"

*handleToNameCMenu.class: cascadeButton
*handleToNameCMenu.static: true
*handleToNameCMenu.name: handleToNameCMenu
*handleToNameCMenu.parent: supportMenu
*handleToNameCMenu.labelString: "Handle to Name"
*handleToNameCMenu.subMenuId: "handleToNamePane"

*supportMenu_b18.class: separator
*supportMenu_b18.static: true
*supportMenu_b18.name: supportMenu_b18
*supportMenu_b18.parent: supportMenu

*getAttrSpaceHandleItem.class: pushButton
*getAttrSpaceHandleItem.static: true
*getAttrSpaceHandleItem.name: getAttrSpaceHandleItem
*getAttrSpaceHandleItem.parent: supportMenu
*getAttrSpaceHandleItem.labelString: "Get Attribute Routing Space Handle..."
*getAttrSpaceHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ATTRIBUTE_SPACE_HANDLE), no_grab);

*getObjectClassItem.class: pushButton
*getObjectClassItem.static: true
*getObjectClassItem.name: getObjectClassItem
*getObjectClassItem.parent: supportMenu
*getObjectClassItem.labelString: "Get Object Class ..."
*getObjectClassItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_CLASS), no_grab);

*getIntSpaceHandleItem.class: pushButton
*getIntSpaceHandleItem.static: true
*getIntSpaceHandleItem.name: getIntSpaceHandleItem
*getIntSpaceHandleItem.parent: supportMenu
*getIntSpaceHandleItem.labelString: "Get Interaction Routing Space Handle..."
*getIntSpaceHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_INTERACTION_SPACE_HANDLE), no_grab);

*supportMenu_b19.class: separator
*supportMenu_b19.static: true
*supportMenu_b19.name: supportMenu_b19
*supportMenu_b19.parent: supportMenu

*advisorySwitchCMenu.class: cascadeButton
*advisorySwitchCMenu.static: true
*advisorySwitchCMenu.name: advisorySwitchCMenu
*advisorySwitchCMenu.parent: supportMenu
*advisorySwitchCMenu.labelString: "Advisory Switch"
*advisorySwitchCMenu.subMenuId: "advisorySwitchPane"

*supportMenu_b9.class: separator
*supportMenu_b9.static: true
*supportMenu_b9.name: supportMenu_b9
*supportMenu_b9.parent: supportMenu

*manualTickItem.class: pushButton
*manualTickItem.static: true
*manualTickItem.name: manualTickItem
*manualTickItem.parent: supportMenu
*manualTickItem.labelString: "Manual Tick..."
*manualTickItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), TICK), no_grab); 

*autoTickItem.class: pushButton
*autoTickItem.name.source: public
*autoTickItem.static: false
*autoTickItem.name: autoTickItem
*autoTickItem.parent: supportMenu
*autoTickItem.labelString: "Enable Automatic Tick..."
*autoTickItem.activateCallback: XmString autoTickItemLabel;\
\
if (autoTickEnabled)  /* Disable autoTick */\
{\
  autoTickEnabled = False;\
\
  autoTickItemLabel = XmStringCreateLocalized ("Enable Automatic Tick...");\
  XtVaSetValues (UxGetWidget (autoTickItem),\
                 XmNlabelString, autoTickItemLabel,\
                 NULL);\
  XmStringFree (autoTickItemLabel);\
\
  do_autoTick (NULL, STOP);\
}\
\
else  /* AutoTick Initialization.  Note that we don't actually start */\
{     /* autoTick until the user selects "Ok" in the prompt dialog box */\
  UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget),\
                                                AUTO_TICK), no_grab);\
}\


*nameToHandlePane.class: rowColumn
*nameToHandlePane.static: true
*nameToHandlePane.name: nameToHandlePane
*nameToHandlePane.parent: supportMenu
*nameToHandlePane.rowColumnType: "menu_pulldown"

*getObjClassHandleItem.class: pushButton
*getObjClassHandleItem.static: true
*getObjClassHandleItem.name: getObjClassHandleItem
*getObjClassHandleItem.parent: nameToHandlePane
*getObjClassHandleItem.labelString: "Get Object Class Handle..."
*getObjClassHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_CLASS_HANDLE), no_grab);  

*getAttrHandleItem.class: pushButton
*getAttrHandleItem.static: true
*getAttrHandleItem.name: getAttrHandleItem
*getAttrHandleItem.parent: nameToHandlePane
*getAttrHandleItem.labelString: "Get Attribute Handle..."
*getAttrHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ATTR_HANDLE), no_grab);  

*getInteractionClassHandleItem.class: pushButton
*getInteractionClassHandleItem.static: true
*getInteractionClassHandleItem.name: getInteractionClassHandleItem
*getInteractionClassHandleItem.parent: nameToHandlePane
*getInteractionClassHandleItem.labelString: "Get Interaction Class Handle..."
*getInteractionClassHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_INTERACTION_CLASS_HANDLE), no_grab);  

*getParamaterHandleItem.class: pushButton
*getParamaterHandleItem.static: true
*getParamaterHandleItem.name: getParamaterHandleItem
*getParamaterHandleItem.parent: nameToHandlePane
*getParamaterHandleItem.labelString: "Get Parameter Handle..."
*getParamaterHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_PARM_HANDLE), no_grab);  

*getObjectHandleItem.class: pushButton
*getObjectHandleItem.static: true
*getObjectHandleItem.name: getObjectHandleItem
*getObjectHandleItem.parent: nameToHandlePane
*getObjectHandleItem.labelString: "Get Object Instance Handle ..."
*getObjectHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_HANDLE), no_grab);

*getSpaceHandleItem.class: pushButton
*getSpaceHandleItem.static: true
*getSpaceHandleItem.name: getSpaceHandleItem
*getSpaceHandleItem.parent: nameToHandlePane
*getSpaceHandleItem.labelString: "Get Routing Space Handle..."
*getSpaceHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_SPACE_HANDLE), no_grab);  

*getDimensionHandleItem.class: pushButton
*getDimensionHandleItem.static: true
*getDimensionHandleItem.name: getDimensionHandleItem
*getDimensionHandleItem.parent: nameToHandlePane
*getDimensionHandleItem.labelString: "Get Dimension Handle..."
*getDimensionHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_DIMENSION_HANDLE), no_grab);

*getTransportationHandleItem.class: pushButton
*getTransportationHandleItem.static: true
*getTransportationHandleItem.name: getTransportationHandleItem
*getTransportationHandleItem.parent: nameToHandlePane
*getTransportationHandleItem.labelString: "Get Transportation Handle..."
*getTransportationHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_TRANSPORTATION_HANDLE), no_grab);

*getOrderingHandleItem.class: pushButton
*getOrderingHandleItem.static: true
*getOrderingHandleItem.name: getOrderingHandleItem
*getOrderingHandleItem.parent: nameToHandlePane
*getOrderingHandleItem.labelString: "Get Ordering Handle..."
*getOrderingHandleItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ORDERING_HANDLE), no_grab);

*handleToNamePane.class: rowColumn
*handleToNamePane.static: true
*handleToNamePane.name: handleToNamePane
*handleToNamePane.parent: supportMenu
*handleToNamePane.rowColumnType: "menu_pulldown"

*getObjClassNameItem.class: pushButton
*getObjClassNameItem.static: true
*getObjClassNameItem.name: getObjClassNameItem
*getObjClassNameItem.parent: handleToNamePane
*getObjClassNameItem.labelString: "Get Object Class Name..."
*getObjClassNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_CLASS_NAME), no_grab); 

*getAttrNameItem.class: pushButton
*getAttrNameItem.static: true
*getAttrNameItem.name: getAttrNameItem
*getAttrNameItem.parent: handleToNamePane
*getAttrNameItem.labelString: "Get Attribute Name..."
*getAttrNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ATTR_NAME), no_grab); 

*getInteractionClassNameItem.class: pushButton
*getInteractionClassNameItem.static: true
*getInteractionClassNameItem.name: getInteractionClassNameItem
*getInteractionClassNameItem.parent: handleToNamePane
*getInteractionClassNameItem.labelString: "Get Interaction Class Name..."
*getInteractionClassNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_INTERACTION_CLASS_NAME), no_grab); 

*getParameterNameItem.class: pushButton
*getParameterNameItem.static: true
*getParameterNameItem.name: getParameterNameItem
*getParameterNameItem.parent: handleToNamePane
*getParameterNameItem.labelString: "Get Parameter Name..."
*getParameterNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_PARM_NAME), no_grab); 

*getObjectNameItem.class: pushButton
*getObjectNameItem.static: true
*getObjectNameItem.name: getObjectNameItem
*getObjectNameItem.parent: handleToNamePane
*getObjectNameItem.labelString: "Get Object Instance Name ..."
*getObjectNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_OBJ_NAME), no_grab);

*getSpaceNameItem.class: pushButton
*getSpaceNameItem.static: true
*getSpaceNameItem.name: getSpaceNameItem
*getSpaceNameItem.parent: handleToNamePane
*getSpaceNameItem.labelString: "Get Routing Space Name..."
*getSpaceNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_SPACE_NAME), no_grab); 

*getDimensionNameItem.class: pushButton
*getDimensionNameItem.static: true
*getDimensionNameItem.name: getDimensionNameItem
*getDimensionNameItem.parent: handleToNamePane
*getDimensionNameItem.labelString: "Get Dimension Name..."
*getDimensionNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_DIMENSION_NAME), no_grab);

*getTransportationNameItem.class: pushButton
*getTransportationNameItem.static: true
*getTransportationNameItem.name: getTransportationNameItem
*getTransportationNameItem.parent: handleToNamePane
*getTransportationNameItem.labelString: "Get Transportation Name..."
*getTransportationNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_TRANSPORTATION_NAME), no_grab);

*getOrderingNameItem.class: pushButton
*getOrderingNameItem.static: true
*getOrderingNameItem.name: getOrderingNameItem
*getOrderingNameItem.parent: handleToNamePane
*getOrderingNameItem.labelString: "Get Ordering Name..."
*getOrderingNameItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), GET_ORDERING_NAME), no_grab);

*advisorySwitchPane.class: rowColumn
*advisorySwitchPane.static: true
*advisorySwitchPane.name: advisorySwitchPane
*advisorySwitchPane.parent: supportMenu
*advisorySwitchPane.rowColumnType: "menu_pulldown"

*enableClassRelevanceAdvisorySwitchItem.class: pushButton
*enableClassRelevanceAdvisorySwitchItem.static: true
*enableClassRelevanceAdvisorySwitchItem.name: enableClassRelevanceAdvisorySwitchItem
*enableClassRelevanceAdvisorySwitchItem.parent: advisorySwitchPane
*enableClassRelevanceAdvisorySwitchItem.labelString: "Enable Class Relevance Advisory Switch..."
*enableClassRelevanceAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_CLASS_RELEVANCE_ADVISORY), no_grab);

*disableClassRelevanceAdvisorySwitchItem.class: pushButton
*disableClassRelevanceAdvisorySwitchItem.static: true
*disableClassRelevanceAdvisorySwitchItem.name: disableClassRelevanceAdvisorySwitchItem
*disableClassRelevanceAdvisorySwitchItem.parent: advisorySwitchPane
*disableClassRelevanceAdvisorySwitchItem.labelString: "Disable Class Relevance Advisory Switch..."
*disableClassRelevanceAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_CLASS_RELEVANCE_ADVISORY), no_grab);

*enableAttributeRelevanceAdvisorySwitchItem.class: pushButton
*enableAttributeRelevanceAdvisorySwitchItem.static: true
*enableAttributeRelevanceAdvisorySwitchItem.name: enableAttributeRelevanceAdvisorySwitchItem
*enableAttributeRelevanceAdvisorySwitchItem.parent: advisorySwitchPane
*enableAttributeRelevanceAdvisorySwitchItem.labelString: "Enable Attribute Relevance Advisory Switch..."
*enableAttributeRelevanceAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_ATTRIBUTE_RELEVANCE_ADVISORY), no_grab);

*disableAttributeRelevanceAdvisorySwitchItem.class: pushButton
*disableAttributeRelevanceAdvisorySwitchItem.static: true
*disableAttributeRelevanceAdvisorySwitchItem.name: disableAttributeRelevanceAdvisorySwitchItem
*disableAttributeRelevanceAdvisorySwitchItem.parent: advisorySwitchPane
*disableAttributeRelevanceAdvisorySwitchItem.labelString: "Disable Attribute Relevance Advisory Switch..."
*disableAttributeRelevanceAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_ATTRIBUTE_RELEVANCE_ADVISORY), no_grab);

*enableAttributeScopeAdvisorySwitchItem.class: pushButton
*enableAttributeScopeAdvisorySwitchItem.static: true
*enableAttributeScopeAdvisorySwitchItem.name: enableAttributeScopeAdvisorySwitchItem
*enableAttributeScopeAdvisorySwitchItem.parent: advisorySwitchPane
*enableAttributeScopeAdvisorySwitchItem.labelString: "Enable Attribute Scope Advisory Switch..."
*enableAttributeScopeAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_ATTRIBUTE_SCOPE_ADVISORY), no_grab);

*disableAttributeScopeAdvisorySwitchItem.class: pushButton
*disableAttributeScopeAdvisorySwitchItem.static: true
*disableAttributeScopeAdvisorySwitchItem.name: disableAttributeScopeAdvisorySwitchItem
*disableAttributeScopeAdvisorySwitchItem.parent: advisorySwitchPane
*disableAttributeScopeAdvisorySwitchItem.labelString: "Disable Attribute Scope Advisory Switch..."
*disableAttributeScopeAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_ATTRIBUTE_SCOPE_ADVISORY), no_grab);

*enableInteractionRelevanceAdvisorySwitchItem.class: pushButton
*enableInteractionRelevanceAdvisorySwitchItem.static: true
*enableInteractionRelevanceAdvisorySwitchItem.name: enableInteractionRelevanceAdvisorySwitchItem
*enableInteractionRelevanceAdvisorySwitchItem.parent: advisorySwitchPane
*enableInteractionRelevanceAdvisorySwitchItem.labelString: "Enable Interaction Relevance Advisory Switch..."
*enableInteractionRelevanceAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), ENABLE_INTERACTION_RELEVANCE_ADVISORY), no_grab);

*disableInteractionRelevanceAdvisorySwitchItem.class: pushButton
*disableInteractionRelevanceAdvisorySwitchItem.static: true
*disableInteractionRelevanceAdvisorySwitchItem.name: disableInteractionRelevanceAdvisorySwitchItem
*disableInteractionRelevanceAdvisorySwitchItem.parent: advisorySwitchPane
*disableInteractionRelevanceAdvisorySwitchItem.labelString: "Disable Interaction Relevance Advisory Switch..."
*disableInteractionRelevanceAdvisorySwitchItem.activateCallback: UxPopupInterface (create_genericPromptDialog (UxWidgetToSwidget (UxWidget), DISABLE_INTERACTION_RELEVANCE_ADVISORY), no_grab);

*fileCMenu.class: cascadeButton
*fileCMenu.static: true
*fileCMenu.name: fileCMenu
*fileCMenu.parent: menubar
*fileCMenu.labelString: "File"
*fileCMenu.mnemonic: "F"
*fileCMenu.subMenuId: "fileMenu"

*federationCMenu.class: cascadeButton
*federationCMenu.static: true
*federationCMenu.name: federationCMenu
*federationCMenu.parent: menubar
*federationCMenu.labelString: "Federation"
*federationCMenu.subMenuId: "federationMenu"
*federationCMenu.mnemonic: "d"

*declarationCMenu.class: cascadeButton
*declarationCMenu.static: true
*declarationCMenu.name: declarationCMenu
*declarationCMenu.parent: menubar
*declarationCMenu.labelString: "Declaration"
*declarationCMenu.mnemonic: "D"
*declarationCMenu.subMenuId: "declarationMenu"

*objectCMenu.class: cascadeButton
*objectCMenu.static: true
*objectCMenu.name: objectCMenu
*objectCMenu.parent: menubar
*objectCMenu.labelString: "Object"
*objectCMenu.mnemonic: "O"
*objectCMenu.subMenuId: "objectMenu"

*ownershipCMenu.class: cascadeButton
*ownershipCMenu.static: true
*ownershipCMenu.name: ownershipCMenu
*ownershipCMenu.parent: menubar
*ownershipCMenu.labelString: "Ownership"
*ownershipCMenu.mnemonic: "w"
*ownershipCMenu.subMenuId: "ownershipMenu"

*timeCMenu.class: cascadeButton
*timeCMenu.static: true
*timeCMenu.name: timeCMenu
*timeCMenu.parent: menubar
*timeCMenu.labelString: "Time"
*timeCMenu.subMenuId: "timeMenu"
*timeCMenu.mnemonic: "T"

*menubar_top_b2.class: cascadeButton
*menubar_top_b2.static: true
*menubar_top_b2.name: menubar_top_b2
*menubar_top_b2.parent: menubar
*menubar_top_b2.labelString: "DDM"
*menubar_top_b2.subMenuId: "DDMmenu"
*menubar_top_b2.mnemonic: "M"

*supportCMenu.class: cascadeButton
*supportCMenu.static: true
*supportCMenu.name: supportCMenu
*supportCMenu.parent: menubar
*supportCMenu.labelString: "Support"
*supportCMenu.subMenuId: "supportMenu"
*supportCMenu.mnemonic: "S"

*form1.class: form
*form1.static: true
*form1.name: form1
*form1.parent: mainWindow1

*statusLine.class: label
*statusLine.name.source: public
*statusLine.static: false
*statusLine.name: statusLine
*statusLine.parent: form1
*statusLine.labelString: ""
*statusLine.rightAttachment: "attach_form"
*statusLine.leftAttachment: "attach_form"
*statusLine.topAttachment: "attach_none"
*statusLine.height: 42

*separator4.class: separator
*separator4.name.source: public
*separator4.static: false
*separator4.name: separator4
*separator4.parent: form1
*separator4.rightAttachment: "attach_form"
*separator4.leftAttachment: "attach_form"
*separator4.topAttachment: "attach_form"
*separator4.topOffset: 43

*outputWindow.class: scrolledWindow
*outputWindow.static: true
*outputWindow.name: outputWindow
*outputWindow.parent: form1
*outputWindow.scrollingPolicy: "application_defined"
*outputWindow.visualPolicy: "variable"
*outputWindow.scrollBarDisplayPolicy: "static"
*outputWindow.bottomAttachment: "attach_form"
*outputWindow.rightAttachment: "attach_form"
*outputWindow.leftAttachment: "attach_form"
*outputWindow.topAttachment: "attach_widget"
*outputWindow.topWidget: "separator4"

*outputWindowText.class: scrolledText
*outputWindowText.name.source: public
*outputWindowText.static: false
*outputWindowText.name: outputWindowText
*outputWindowText.parent: outputWindow
*outputWindowText.width: 312
*outputWindowText.height: 107
*outputWindowText.scrollHorizontal: "false"
*outputWindowText.wordWrap: "true"
*outputWindowText.cursorPositionVisible: "false"
*outputWindowText.editMode: "multi_line_edit"
*outputWindowText.editable: "false"

*messageWindowMenu.class: rowColumn
*messageWindowMenu.static: true
*messageWindowMenu.name: messageWindowMenu
*messageWindowMenu.parent: outputWindowText
*messageWindowMenu.rowColumnType: "menu_popup"
*messageWindowMenu.menuAccelerator: "Shift <KeyUp>F10"

*menu1_p1_title.class: label
*menu1_p1_title.static: true
*menu1_p1_title.name: menu1_p1_title
*menu1_p1_title.parent: messageWindowMenu
*menu1_p1_title.labelString: "MESSAGES"

*titleSeparator.class: separator
*titleSeparator.static: true
*titleSeparator.name: titleSeparator
*titleSeparator.parent: messageWindowMenu

*clearWindowItem.class: pushButton
*clearWindowItem.static: true
*clearWindowItem.name: clearWindowItem
*clearWindowItem.parent: messageWindowMenu
*clearWindowItem.labelString: "Clear Window"
*clearWindowItem.mnemonic: "C"
*clearWindowItem.activateCallback: XmTextSetString (UxGetWidget (outputWindowText), "");

*insertMarkerItem.class: pushButton
*insertMarkerItem.static: true
*insertMarkerItem.name: insertMarkerItem
*insertMarkerItem.parent: messageWindowMenu
*insertMarkerItem.labelString: "Insert Marker" 
*insertMarkerItem.activateCallback: {\
XmTextPosition  lastPosition;\
\
lastPosition = XmTextGetLastPosition (UxGetWidget (outputWindowText));\
XmTextInsert (UxGetWidget (outputWindowText), lastPosition, "\n<+>");\
\
}
*insertMarkerItem.mnemonic: "I"

