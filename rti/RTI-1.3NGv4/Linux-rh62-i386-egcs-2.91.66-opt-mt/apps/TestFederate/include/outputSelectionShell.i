! UIMX ascii 2.9 key: 9088                                                      

*outputSelectionShell.class: applicationShell
*outputSelectionShell.classinc:
*outputSelectionShell.classspec:
*outputSelectionShell.classmembers:
*outputSelectionShell.classconstructor:
*outputSelectionShell.classdestructor:
*outputSelectionShell.gbldecl: #include <stdio.h>\
\
extern swidget  create_fileSelectionShell(swidget, swidget);\
\
extern swidget  outputSelectionSwidget;\
extern swidget  statusLine;\
\
extern void updateStatus();\
extern void outputMsg (char *);\
\
\
extern Boolean  outputRecording;\
extern Boolean  outputDisplay;\
extern FILE    *outputFile;\
char           *outputFilename;\
\
swidget fileSelectionSwidget;\
\

*outputSelectionShell.ispecdecl:
*outputSelectionShell.funcdecl: swidget create_outputSelectionShell(swidget UxParent)
*outputSelectionShell.funcname: create_outputSelectionShell
*outputSelectionShell.funcdef: "swidget", "<create_outputSelectionShell>(%)"
*outputSelectionShell.argdecl: swidget UxParent;
*outputSelectionShell.arglist: UxParent
*outputSelectionShell.arglist.UxParent: "swidget", "%UxParent%"
*outputSelectionShell.icode: outputFilename = "./tf_output.log";
*outputSelectionShell.fcode: return(rtrn);\

*outputSelectionShell.auxdecl:
*outputSelectionShell.static: true
*outputSelectionShell.name: outputSelectionShell
*outputSelectionShell.parent: NO_PARENT
*outputSelectionShell.parentExpression: UxParent
*outputSelectionShell.x: 403
*outputSelectionShell.y: 218
*outputSelectionShell.width: 327
*outputSelectionShell.height: 140
*outputSelectionShell.title: "Output Selection"
*outputSelectionShell.popupCallback: {\
XmToggleButtonSetState (UxGetWidget (windowOutputBtn),\
                        outputDisplay, False);\
\
XmToggleButtonSetState (UxGetWidget (fileOutputBtn),\
                        outputRecording, False);\
\
XmTextFieldSetString (UxGetWidget (outputFileField),\
                      outputFilename);\
}

*bulletinBoard2.class: bulletinBoard
*bulletinBoard2.static: true
*bulletinBoard2.name: bulletinBoard2
*bulletinBoard2.parent: outputSelectionShell
*bulletinBoard2.resizePolicy: "resize_none"
*bulletinBoard2.unitType: "pixels"
*bulletinBoard2.x: 62
*bulletinBoard2.y: 16
*bulletinBoard2.width: 327
*bulletinBoard2.height: 140
*bulletinBoard2.marginWidth: 0

*windowOutputBtn.class: toggleButton
*windowOutputBtn.static: true
*windowOutputBtn.name: windowOutputBtn
*windowOutputBtn.parent: bulletinBoard2
*windowOutputBtn.x: 9
*windowOutputBtn.y: 10
*windowOutputBtn.width: 123
*windowOutputBtn.height: 25
*windowOutputBtn.labelString: "Output to Window"
*windowOutputBtn.alignment: "alignment_beginning"

*fileOutputBtn.class: toggleButton
*fileOutputBtn.static: true
*fileOutputBtn.name: fileOutputBtn
*fileOutputBtn.parent: bulletinBoard2
*fileOutputBtn.x: 9
*fileOutputBtn.y: 36
*fileOutputBtn.width: 125
*fileOutputBtn.height: 25
*fileOutputBtn.labelString: "Output to File:"
*fileOutputBtn.alignment: "alignment_beginning"

*outputFileField.class: textField
*outputFileField.static: true
*outputFileField.name: outputFileField
*outputFileField.parent: bulletinBoard2
*outputFileField.x: 135
*outputFileField.y: 34
*outputFileField.width: 113
*outputFileField.height: 31

*outputFileSelectBtn.class: pushButton
*outputFileSelectBtn.static: true
*outputFileSelectBtn.name: outputFileSelectBtn
*outputFileSelectBtn.parent: bulletinBoard2
*outputFileSelectBtn.x: 256
*outputFileSelectBtn.y: 34
*outputFileSelectBtn.width: 54
*outputFileSelectBtn.height: 31
*outputFileSelectBtn.labelString: "Select"
*outputFileSelectBtn.activateCallback: {\
fileSelectionSwidget = create_fileSelectionShell (NULL, outputFileField);\
UxPopupInterface (fileSelectionSwidget, no_grab);\
}

*separator1.class: separator
*separator1.static: true
*separator1.name: separator1
*separator1.parent: bulletinBoard2
*separator1.x: -6
*separator1.y: 80
*separator1.width: 333
*separator1.height: 7

*outputSelectionOkBtn.class: pushButton
*outputSelectionOkBtn.static: true
*outputSelectionOkBtn.name: outputSelectionOkBtn
*outputSelectionOkBtn.parent: bulletinBoard2
*outputSelectionOkBtn.x: 20
*outputSelectionOkBtn.y: 100
*outputSelectionOkBtn.width: 70
*outputSelectionOkBtn.height: 26
*outputSelectionOkBtn.labelString: "Ok"
*outputSelectionOkBtn.activateCallback: {\
/* Retrieve the new values */\
\
outputDisplay   = XmToggleButtonGetState (UxGetWidget (windowOutputBtn));\
outputRecording = XmToggleButtonGetState (UxGetWidget (fileOutputBtn));\
\
if (outputFilename != NULL)\
  XtFree (outputFilename);\
\
outputFilename =\
  XmTextFieldGetString (UxGetWidget (outputFileField));\
\
if ((outputRecording) && (outputFile == NULL))\
{\
  outputFile = fopen (outputFilename, "w");\
\
  /* Make sure we were able to open the file. */\
  if (outputFile == NULL)\
  {\
    char msg[150];\
\
    sprintf (msg, "Output Recording:  Unable to open file '%s'",\
             outputFilename);\
    outputMsg (msg);\
\
    outputRecording = False;\
\
    /* Make the "Output Display" dialog box disappear. */\
    UxPopdownInterface (outputSelectionSwidget);\
\
    return;\
  }\
}\
else if ((!outputRecording) && (outputFile != NULL))\
{\
  fclose (outputFile);\
  outputFile = NULL;\
}\
\
\
updateStatus();\
\
/* Turn off the output file field and the Select button\
   while recording the window output to file.  */\
 \
XtVaSetValues (UxGetWidget (outputFileField),\
               XmNsensitive, !outputRecording,\
               NULL);\
XtVaSetValues (UxGetWidget (outputFileSelectBtn),\
               XmNsensitive, !outputRecording,\
               NULL);\
\
/* Make the "Output Display" dialog box disappear. */\
UxPopdownInterface (outputSelectionSwidget);\
}

*outputSelectionCancelBtn.class: pushButton
*outputSelectionCancelBtn.static: true
*outputSelectionCancelBtn.name: outputSelectionCancelBtn
*outputSelectionCancelBtn.parent: bulletinBoard2
*outputSelectionCancelBtn.x: 130
*outputSelectionCancelBtn.y: 100
*outputSelectionCancelBtn.width: 70
*outputSelectionCancelBtn.height: 26
*outputSelectionCancelBtn.labelString: "Cancel"
*outputSelectionCancelBtn.activateCallback: {\
UxPopdownInterface (outputSelectionSwidget);\
}

*outputSelectionHelpBtn.class: pushButton
*outputSelectionHelpBtn.static: true
*outputSelectionHelpBtn.name: outputSelectionHelpBtn
*outputSelectionHelpBtn.parent: bulletinBoard2
*outputSelectionHelpBtn.x: 240
*outputSelectionHelpBtn.y: 100
*outputSelectionHelpBtn.width: 70
*outputSelectionHelpBtn.height: 26
*outputSelectionHelpBtn.labelString: "Help"

