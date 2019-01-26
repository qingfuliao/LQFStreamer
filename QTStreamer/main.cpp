#include "QTStreamer.h"
#include <QtWidgets/QApplication>

int main2(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTStreamer w;
	w.show();
	return a.exec();
}
// extern int test_streamer(void);
// int main(void)
// {
// 	test_streamer();
// 	return 0;
// }

extern int test_streamer();
extern int test_server(int argc, char *argv[]);
extern int test_librtmp_pull(int argc, char* argv[]);
#include <iostream>
int main(int argc, char *argv[])
{
// 	int ret = test_streamer();
// 	std::cout << "ret = " << ret<< std::endl;

	test_librtmp_pull(argc, argv);
	system("pause");
	return 0;
}
