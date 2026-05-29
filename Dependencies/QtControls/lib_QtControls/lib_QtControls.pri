HEADERS += \
    ../src/CustomCalendar/CustomCalendar.h \
    ../src/CustomComboBox/CustomComboBox.h \
    ../src/CustomCalendar/CustomDateTimeEdit.h \
    ../src/CustomCalendar/CustomIntervalEdit.h \
    ../src/ViewList/TreeViewColumnResizer.h \
    ../src/FileExplorer/FileDialogWidget.h \
    ../src/FileExplorer/ThumbStructs.h \
    ../src/FileExplorer/fileDialogTree.h \
    ../src/FileExplorer/thumbnailCreator.h \
    ../src/FileExplorer/dirDialogTree.h \
    ../src/ViewList/dropIndicatorProxyStyle.h \
    ../src/ViewList/headerview.h \
    ../src/ViewList/hotKeyEditor.h \
    ../src/ViewList/styleItemDelegate.h \
    ../src/ViewList/treeitem.h \
    ../src/ViewList/treemodel.h \
    ../src/ViewList/treeview.h \
    ../src/Logger/Logger.h \
    ../src/DBOperations/DBConnection.h \
    ../src/DBOperations/DBConnector.h \    
    ../src/TCEditWidget/TCEditWidget.h \
    ../src/TCEditWidget/TCOperations.h \    
    ../src/ViewList/writablelocation.h \
    ../src/ViewList/ItemComboBox.h \
    ../src/ViewList/customDragViewList.h \
    ../src/BramPlayerWidget/AspectRatioSingleItemLayout.h\
    ../src/CheckComboBox/checkCombobox.h\
    ../src/Application/application.h\
    ../src/ViewList/itemTextEdit.h


SOURCES += \
    ../src/CustomCalendar/CustomCalendar.cpp \
    ../src/CustomComboBox/CustomComboBox.cpp \
    ../src/CustomCalendar/CustomDateTimeEdit.cpp \
    ../src/CustomCalendar/CustomIntervalEdit.cpp \
    ../src/ViewList/TreeViewColumnResizer.cpp \
    ../src/FileExplorer/FileDialogWidget.cpp \
    ../src/FileExplorer/fileDialogTree.cpp \
    ../src/FileExplorer/thumbnailCreator.cpp \
    ../src/FileExplorer/dirDialogTree.cpp \
    ../src/ViewList/dropIndicatorProxyStyle.cpp \
    ../src/ViewList/headerview.cpp \
    ../src/ViewList/hotKeyEditor.cpp \
    ../src/ViewList/styleItemDelegate.cpp \
    ../src/ViewList/treeitem.cpp \
    ../src/ViewList/treemodel.cpp \
    ../src/ViewList/treeview.cpp \
    ../src/Logger/Logger.cpp \
    ../src/DBOperations/DBConnection.cpp \
    ../src/DBOperations/DBConnector.cpp \    
    ../src/TCEditWidget/TCEditWidget.cpp \
    ../src/TCEditWidget/TCOperations.cpp \    
    ../src/ViewList/writablelocation.cpp \
    ../src/ViewList/ItemComboBox.cpp \
    ../src/ViewList/customDragViewList.cpp \
    ../src/BramPlayerWidget/AspectRatioSingleItemLayout.cpp\
    ../src/CheckComboBox/checkCombobox.cpp\
    ../src/Application/application.cpp\
    ../src/ViewList/itemTextEdit.cpp

FORMS += \
    ../src/CustomCalendar/ui/CustomCalendar.ui \
    ../src/CustomComboBox/CustomComboBox.ui \
    ../src/CustomCalendar/ui/CustomIntervalEdit.ui \
    ../src/FileExplorer/FileDialogWidget.ui \
    ../src/BramPlayerWidget/BramPlayerAdvanced.ui

RESOURCES += \
    ../Resources/res_libQtControls.qrc

INCLUDEPATH	+= ../src/Logger \
                   ../src/DBOperations \
                   ../src/TCEditWidget \
                   ../src/ViewList\
                   ../src/BramPlayerWidget\
                   ../src/FileExplorer\
                   ../src/CustomCalendar\
                   ../src/CustomComboBox


TRANSLATIONS += ../Resources/lang/QtControls_ru.ts

