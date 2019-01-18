#include "QTStreamer.h"
#include <QtWidgets/QApplication>

int main2(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTStreamer w;
	w.show();
	return a.exec();
}

extern int test_server(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	int ret = test_server(argc, argv);
	//cout << "ret = " << ret<< endl;
	system("pause");
	return 0;
}
#include <iostream>
#include "Util/logger.h"

using namespace std;
using namespace toolkit;

class TestLog
{
public:
	template<typename T>
	TestLog(const T &t) {
		_ss << t;
	};
	~TestLog() {};

	//通过此友元方法，可以打印自定义数据类型
	friend ostream& operator<<(ostream& out, const TestLog& obj) {
		return out << obj._ss.str();
	}
private:
	stringstream _ss;
};

int main3() {
	//初始化日志系统
	Logger::Instance().add(std::make_shared<ConsoleChannel>());
	Logger::Instance().add(std::make_shared<FileChannel>("FileChannel", "test.log"));
	Logger::Instance().setWriter(std::make_shared<AsyncLogWriter>());

	//ostream支持的数据类型都支持,可以通过友元的方式打印自定义类型数据
	TraceL << "object int" << TestLog((int)1) << endl;
	DebugL << "object short:" << TestLog((short)2) << endl;
	InfoL << "object float:" << TestLog((float)3.12345678) << endl;
	WarnL << "object double:" << TestLog((double)4.12345678901234567) << endl;
	ErrorL << "object void *:" << TestLog((void *)0x12345678) << endl;
	ErrorL << "object string:" << TestLog("test string") << endl;


	//这是ostream原生支持的数据类型
	TraceL << "int" << (int)1 << endl;
	DebugL << "short:" << (short)2 << endl;
	InfoL << "float:" << (float)3.12345678 << endl;
	WarnL << "double:" << (double)4.12345678901234567 << endl;
	ErrorL << "void *:" << (void *)0x12345678 << endl;
	//根据RAII的原理，此处不需要输入 endl，也会在被函数栈pop时打印log
	ErrorL << "without endl!";
	system("pause");
	return 0;
}
