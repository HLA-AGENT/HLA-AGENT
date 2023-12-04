! UIMX ascii 2.9 key: 5180                                                      

*fileSelectionShell.class: applicationShell
*fileSelectionShell.classinc:
*fileSelectionShell.classspec:
*fileSelectionShell.classmembers:
*fileSelectionShell.classconstructor:
*fileSelectionShell.classdestructor:
*fileSelectionShell.gbldecl: #include <stdio.h>\
\
extern swidget fileSelectionSwidget;\

*fileSelectionShell.ispecdecl:
*fileSelectionShell.funcdecl: swidget create_fileSelectionShell(swidget UxParent, swidget outputFileField)
*fileSelectionShell.funcname: create_fileSelectionShell
*fileSelectionShell.funcdef: "swidget", "<create_fileSelectionShell>(%)"
*fileSelectionShell.argdecl: swidget UxParent;\
swidget outputFileField;
*fileSelectionShell.arglist: UxParent, outputFileField
*fileSelectionShell.arglist.UxParent: "swidget", "%UxParent%"
*fileSelectionShell.arglist.outputFileField: "swidget", "%outputFileField%"
*fileSelectionShell.icode:
*fileSelectionShell.fcode: return(rtrn);\

*fileSelectionShell.auxdecl:
*fileSelectionShell.static: true
*fileSelectionShell.name: fileSelectionShell
*fileSelectionShell.parent: NO_PARENT
*fileSelectionShell.parentExpression: UxParent
*fileSelectionShell.x: 391
*fileSelectionShell.y: 189
*fileSelectionShell.width: 339
*fileSelectionShell.height: 327
*fileSelectionShell.title: "Select Output File"

*fileSelectionBoxDialog.class: fileSelectionBox
*fileSelectionBoxDialog.static: true
*fileSelectionBoxDialog.name: fileSelectionBoxDialog
*fileSelectionBoxDialog.parent: fileSelectionShell
*fileSelectionBoxDialog.resizePolicy: "resize_none"
*fileSelectionBoxDialog.unitType: "pixels"
*fileSelectionBoxDialog.x: 82
*fileSelectionBoxDialog.y: 45
*fileSelectionBoxDialog.width: 339
*fileSelectionBoxDialog.height: 327
*fileSelectionBoxDialog.pattern: "*.log"
*fileSelectionBoxDialog.cancelCallback: {\
UxPopdownInterface (fileSelectionSwidget);\
}
*fileSelectionBoxDialog.okCallback: {\
char *filename;\
XmFileSelectionBoxCallbackStruct *cbs =\
  (XmFileSelectionBoxCallbackStruct *) UxCallbackArg;\
\
UxPopdownInterface (fileSelectionSwidget);\
\
if (!XmStringGetLtoR (cbs->value, XmFONTLIST_DEFAULT_TAG,\
                      &filename))\
  return; /* must have been an internal error */\
\
if (!*filename) /* nothing typed */\
{\
  printf ("No file selected.\n");\
  XtFree (filename);  /* even "" is an allocated byte */\
  return;\
}\
\
XtVaSetValues (UxGetWidget (outputFileField),\
               XmNvalue, filename,\
               NULL);\
\
XtFree (filename);\
\
\
}

