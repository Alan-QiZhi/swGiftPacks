#pragma once

namespace rc17
{
	class Execute
	{
	public:
		static bool init();
		static void run(HObject _depthImage);
	private:
		static void initPos();
		static void initHWindow();
		static void initSerialPort();
		static void initSocket();
		static void setCameraParam();
		static void getPillarCoor();
		static void saucerTrack();
	};
}