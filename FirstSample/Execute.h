#pragma once

namespace rc17
{
	class Execute
	{
	public:
		bool init();
		void run(HObject _depthImage);
	private:
		void initPos();
		void initHWindow();
		void initSerialPort();
		void initSocket();
		void setCameraParam();
		void getPillarCoor();
		void saucerTrack();
	};
}