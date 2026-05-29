#include "writablelocation.h"
#include "QStandardPaths"
#include "QApplication"

QString getWritableLocation()
{
    static QString writableLoc;
    if (writableLoc.isEmpty())
    {
        writableLoc = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QString appName = qApp->applicationName();
        if (!appName.isEmpty())
        {
            writableLoc = writableLoc.left(writableLoc.indexOf(appName) - 1);
        }
        writableLoc += "/BRAM Technologies/";
    }
    return writableLoc;
}

