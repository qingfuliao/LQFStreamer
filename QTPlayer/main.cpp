#include "QTPlayer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTPlayer w;
	w.show();
	return a.exec();
}
