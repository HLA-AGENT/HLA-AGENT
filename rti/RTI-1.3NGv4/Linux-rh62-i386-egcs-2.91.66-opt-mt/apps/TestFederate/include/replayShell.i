! UIMX ascii 2.9 key: 9417                                                      

*replayShell.class: applicationShell
*replayShell.classinc:
*replayShell.classspec:
*replayShell.classmembers:
*replayShell.classconstructor:
*replayShell.classdestructor:
*replayShell.gbldecl: #include <stdio.h>\
\
extern swidget create_fileSelectionShell(swidget, swidget);\
\
extern swidget replaySwidget;\
extern swidget fileSelectionSwidget;\
extern swidget recordItem;\
extern swidget replayItem;\
\
extern void updateStatus();\
extern void outputMsg (char *);\
extern void replayCommands (XtPointer, XtIntervalId *);\
\
extern Boolean  commandReplay;\
extern FILE    *commandFile;\
char           *replayFilename;
*replayShell.ispecdecl:
*replayShell.funcdecl: swidget create_replayShell(swidget UxParent)
*replayShell.funcname: create_replayShell
*replayShell.funcdef: "swidget", "<create_replayShell>(%)"
*replayShell.argdecl: swidget UxParent;
*replayShell.arglist: UxParent
*replayShell.arglist.UxParent: "swidget", "%UxParent%"
*replayShell.icode: replayFilename = NULL;
*replayShell.fcode: return(rtrn);\

*replayShell.auxdecl:
*replayShell.static: true
*replayShell.name: replayShell
*replayShell.parent: NO_PARENT
*replayShell.parentExpression: UxParent
*replayShell.x: 392
*replayShell.y: 204
*replayShell.width: 327
*replayShell.height: 133
*replayShell.title: "Replay File"
*replayShell.popupCallback: {\
if (replayFilename != NULL)\
  XtFree (replayFilename);\
\
replayFilename =\
  XmTextFieldGetString (UxGetWidget (replayFileField));\
}

*bulletinBoard2.class: bulletinBoard
*bulletinBoard2.static: true
*bulletinBoard2.name: bulletinBoard2
*bulletinBoard2.parent: replayShell
*bulletinBoard2.resizePolicy: "resize_none"
*bulletinBoard2.unitType: "pixels"
*bulletinBoard2.x: 62
*bulletinBoard2.y: 16
*bulletinBoard2.width: 327
*bulletinBoard2.height: 272
*bulletinBoard2.marginWidth: 0

*replayFileField.class: textField
*replayFileField.static: true
*replayFileField.name: replayFileField
*replayFileField.parent: bulletinBoard2
*replayFileField.x: 117
*replayFileField.y: 20
*replayFileField.width: 133
*replayFileField.height: 31
*replayFileField.text: "./tf_command.log"

*replaySelectBtn.class: pushButton
*replaySelectBtn.static: true
*replaySelectBtn.name: replaySelectBtn
*replaySelectBtn.parent: bulletinBoard2
*replaySelectBtn.x: 258
*replaySelectBtn.y: 20
*replaySelectBtn.width: 54
*replaySelectBtn.height: 31
*replaySelectBtn.labelString: "Select"
*replaySelectBtn.activateCallback: {\
fileSelectionSwidget = create_fileSelectionShell (NULL, replayFileField);\
UxPopupInterface (fileSelectionSwidget, no_grab);\
}

*separator1.class: separator
*separator1.static: true
*separator1.name: separator1
*separator1.parent: bulletinBoard2
*separator1.x: -4
*separator1.y: 66
*separator1.width: 330
*separator1.height: 7

*replayOkBtn.class: pushButton
*replayOkBtn.static: true
*replayOkBtn.name: replayOkBtn
*replayOkBtn.parent: bulletinBoard2
*replayOkBtn.x: 22
*replayOkBtn.y: 86
*replayOkBtn.width: 70
*replayOkBtn.height: 26
*replayOkBtn.labelString: "Ok"
*replayOkBtn.activateCallback: {\
XmString  replayItemLabel;\
\
/* Retrieve the filename from the Command Replay Dialog Box */\
if (replayFilename != NULL)\
  XtFree (replayFilename);\
replayFilename =\
  XmTextFieldGetString (UxGetWidget (replayFileField));\
\
commandFile = fopen (replayFilename, "r");\
\
/* Make sure we were able to open the file. */\
if (commandFile == NULL)\
{\
  char msg[150];\
\
  sprintf (msg, "Command Replay:  Unable to open file '%s'",\
           replayFilename);\
  outputMsg (msg); \
  return;\
}\
\
/* Ok, we're ready to go.  Perform all of the interface\
   changes related to command replay, then return. */\
replayItemLabel =\
  XmStringCreateLocalized ("Stop Command Replay");\
XtVaSetValues (UxGetWidget (replayItem),\
               XmNlabelString, replayItemLabel,\
               NULL);\
XmStringFree (replayItemLabel);\
\
commandReplay = True;\
updateStatus(); \
\
/* Disable "Command Record" while command replay is in\
   progress.  It will be re-enabled when the user selects\
   "Stop Command Replay" from the "File" menu. */\
\
XtVaSetValues (UxGetWidget (recordItem),\
               XmNsensitive, False,\
               NULL);\
\
/* Schedule the first command replay */\
XtAppAddTimeOut (UxAppContext, 0, replayCommands, NULL);\
\
/* Make the "Command Replay" dialog box disappear. */\
UxPopdownInterface (replaySwidget);\
\
}

*replayCancelBtn.class: pushButton
*replayCancelBtn.static: true
*replayCancelBtn.name: replayCancelBtn
*replayCancelBtn.parent: bulletinBoard2
*replayCancelBtn.x: 132
*replayCancelBtn.y: 86
*replayCancelBtn.width: 70
*replayCancelBtn.height: 26
*replayCancelBtn.labelString: "Cancel"
*replayCancelBtn.activateCallback: {\
XtVaSetValues (UxGetWidget (replayFileField),\
               XmNvalue, replayFilename,\
               NULL);\
\
UxPopdownInterface (replaySwidget);\
}

*reaplyHelpBtn.class: pushButton
*reaplyHelpBtn.static: true
*reaplyHelpBtn.name: reaplyHelpBtn
*reaplyHelpBtn.parent: bulletinBoard2
*reaplyHelpBtn.x: 242
*reaplyHelpBtn.y: 86
*reaplyHelpBtn.width: 70
*reaplyHelpBtn.height: 26
*reaplyHelpBtn.labelString: "Help"

*replayLabel.class: label
*replayLabel.static: true
*replayLabel.name: replayLabel
*replayLabel.parent: bulletinBoard2
*replayLabel.x: 16
*replayLabel.y: 22
*replayLabel.width: 97
*replayLabel.height: 25
*replayLabel.labelString: "File to Replay:"

