# 开发环境
visual studio 2015 + QT5.10.1 + WIN10，程序明确编译为32bit debug版本
# 功能
1. 屏幕录制
2. 支持rtmp直播

# 改进方向
-  将转发功能集成到可视界面
 
# 功能测试
1. 测试码流转发功能，在main函数只调用test_server()函数，自动拉取rtmp://live.hkstv.hk.lxdns.com/live/hks1 码流进行转发，转发后的地址参考test_server.cpp的254行左右
	 - hls地址 : http://127.0.0.1/live/0/hls.m3u8
    - http-flv地址 : http://127.0.0.1/live/0.flv
     - rtsp地址 : rtsp://127.0.0.1/live/0
    - rtmp地址 : rtmp://127.0.0.1/live/0
2. 测试带界面的功能
	使用
```
int main2(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTStreamer w;
	w.show();
	return a.exec();
}
```
# 说明
- 开源库ZLMediaKit和ZLToolKit版权归原作者所有，商用时请联系该作者。