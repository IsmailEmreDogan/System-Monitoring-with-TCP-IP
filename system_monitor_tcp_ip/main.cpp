#include "windowclient.h"
#include "windowserver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    windowclient clientWindowInstance;  // İsimleri daha anlaşılır yapabilirsiniz.
    clientWindowInstance.show();

    windowserver serverWindowInstance;
    serverWindowInstance.show();
    return a.exec();
}
