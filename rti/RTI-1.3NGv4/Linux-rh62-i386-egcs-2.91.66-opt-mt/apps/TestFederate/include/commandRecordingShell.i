! UIMX ascii 2.9 key: 8447                                                      

*commandRecordingShell.class: applicationShell
*commandRecordingShell.classinc:
*commandRecordingShell.classspec:
*commandRecordingShell.classmembers:
*commandRecordingShell.classconstructor:
*commandRecordingShell.classdestructor:
*commandRecordingShell.gbldecl: #include <stdio.h>\
\
extern swidget create_fileSelectionShell(swidget, swidget);\
\
extern swidget commandRecordingSwidget;\
extern swidget fileSelectionSwidget;\
extern swidget recordItem;\
extern swidget replayItem;\
\
extern void updateStatus();\
extern void outputMsg (char *);\
\
extern Boolean  commandRecording;\
extern FILE    *commandFile;\
char           *commandFilename;
*commandRecordingShell.ispecdecl:
*commandRecordingShell.funcdecl: swidget create_commandRecordingShell(swidget UxParent)
*commandRecordingShell.funcname: create_commandRecordingShell
*commandRecordingShell.funcdef: "swidget", "<create_commandRecordingShell>(%)"
*commandRecordingShell.argdecl: swidget UxParent;
*commandRecordingShell.arglist: UxParent
*commandRecordingShell.arglist.UxParent: "swidget", "%UxParent%"
*commandRecordingShell.icode: commandFilename = NULL;
*commandRecordingShell.fcode: return(rtrn);\

*commandRecordingShell.auxdecl:
*commandRecordingShell.static: true
*commandRecordingShell.name: commandRecordingShell
*commandRecordingShell.parent: NO_PARENT
*commandRecordingShell.parentExpression: UxParent
*commandRecordingShell.x: 419
*commandRecordingShell.y: 251
*commandRecordingShell.width: 327
*commandRecordingShell.height: 133
*commandRecordingShell.title: "Command Recording"
*commandRecordingShell.popupCallback: {\
if (commandFilename != NULL)\
  XtFree (commandFilename);\
\
commandFilename =\
  XmTextFieldGetString (UxGetWidget (commandFileField));\
}

*bulletinBoard2.class: bulletinBoard
*bulletinBoard2.static: true
*bulletinBoard2.name: bulletinBoard2
*bulletinBoard2.parent: commandRecordingShell
*bulletinBoard2.resizePolicy: "resize_none"
*bulletinBoard2.unitType: "pixels"
*bulletinBoard2.x: 62
*bulletinBoard2.y: 16
*bulletinBoard2.width: 327
*bulletinBoard2.height: 272
*bulletinBoard2.marginWidth: 0

*commandFileField.class: textField
*commandFileField.static: true
*commandFileField.name: commandFileField
*commandFileField.parent: bulletinBoard2
*commandFileField.x: 117
*commandFileField.y: 20
*commandFileField.width: 133
*commandFileField.height: 31
*commandFileField.text: "./tf_command.log"

*commandRecordingSelectBtn.class: pushButton
*commandRecordingSelectBtn.static: true
*commandRecordingSelectBtn.name: commandRecordingSelectBtn
*commandRecordingSelectBtn.parent: bulletinBoard2
*commandRecordingSelectBtn.x: 258
*commandRecordingSelectBtn.y: 20
*commandRecordingSelectBtn.width: 54
*commandRecordingSelectBtn.height: 31
*commandRecordingSelectBtn.labelString: "Select"
*commandRecordingSelectBtn.activateCallback: {\
fileSelectionSwidget = create_fileSelectionShell (NULL, commandFileField);\
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

*commandRecordingLabel.class: label
*commandRecordingLabel.static: true
*commandRecordingLabel.name: commandRecordingLabel
*commandRecordingLabel.parent: bulletinBoard2
*commandRecordingLabel.x: 16
*commandRecordingLabel.y: 22
*commandRecordingLabel.width: 97
*commandRecordingLabel.height: 25
*commandRecordingLabel.labelString: "Output to File:"

*commandRecordingOkBtn.class: pushButton
*commandRecordingOkBtn.static: true
*commandRecordingOkBtn.name: commandRecordingOkBtn
*commandRecordingOkBtn.parent: bulletinBoard2
*commandRecordingOkBtn.x: 17
*commandRecordingOkBtn.y: 87
*commandRecordingOkBtn.height: 26
*commandRecordingOkBtn.labelString: "Ok"
*commandRecordingOkBtn.activateCallback: {\
XmString  recordItemLabel;\
\
/* Retrieve the filename from the Command Recording Dialog Box */\
if (commandFilename != NULL)\
  XtFree (commandFilename);\
commandFilename =\
  XmTextFieldGetString (UxGetWidget (commandFileField));\
\
commandFile = fopen (commandFilename, "w");\
\
/* Make sure we were able to open the file. */\
if (commandFile == NULL)\
{\
  char msg[150];\
\
  sprintf (msg, "Command Recording:  Unable to open file '%s'",\
           commandFilename);\
  outputMsg (msg); \
}\
else\
{\
  /* Ok, we're ready to go.  Perform all of the interface\
     changes related to command recording, then return. */\
  recordItemLabel =\
    XmStringCreateLocalized ("Stop Command Recording");\
  XtVaSetValues (UxGetWidget (recordItem),\
                 XmNlabelString, recordItemLabel,\
                 NULL);\
  XmStringFree (recordItemLabel);\
\
  commandRecording = True;\
  updateStatus(); \
\
  /* Disable "File Replay" while command recording is in\
     progress.  It will be re-enabled when the user selects\
     "Stop Command Recording" from the "File" menu. */\
\
  XtVaSetValues (UxGetWidget (replayItem),\
                 XmNsensitive, False,\
                 NULL);\
}\
\
/* Make the "Command Recording" dialog box disappear. */\
UxPopdownInterface (commandRecordingSwidget);\
\
}
*commandRecordingOkBtn.width: 70

*commandRecordingCancelBtn.class: pushButton
*commandRecordingCancelBtn.static: true
*commandRecordingCancelBtn.name: commandRecordingCancelBtn
*commandRecordingCancelBtn.parent: bulletinBoard2
*commandRecordingCancelBtn.x: 127
*commandRecordingCancelBtn.y: 87
*commandRecordingCancelBtn.width: 70
*commandRecordingCancelBtn.height: 26
*commandRecordingCancelBtn.labelString: "Cancel"
*commandRecordingCancelBtn.activateCallback: {\
XtVaSetValues (UxGetWidget (commandFileField),\
               XmNvalue, commandFilename,\
               NULL);\
\
UxPopdownInterface (commandRecordingSwidget);\
}

*commandRecordingHelpBtn.class: pushButton
*commandRecordingHelpBtn.static: true
*commandRecordingHelpBtn.name: commandRecordingHelpBtn
*commandRecordingHelpBtn.parent: bulletinBoard2
*commandRecordingHelpBtn.x: 237
*commandRecordingHelpBtn.y: 87
*commandRecordingHelpBtn.width: 70
*commandRecordingHelpBtn.height: 26
*commandRecordingHelpBtn.labelString: "Help"

