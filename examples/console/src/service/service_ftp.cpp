#include <chrono>
#include <future>
#include <thread>

#include <wxlib/flux.h>
#include <action/action_types.h>
#include <service/service_ftp.h>

void FtpService::onUploadFtp(std::string&& filename)
{
	using wxlib::flux::Dispatcher;
	using wxlib::flux::Action;

	future_ = std::async(std::launch::async, [] () {
		
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		int percentDone = 0;

		for (int i = 0; i < 10; ++i) {
			percentDone += 10;
			Dispatcher::instance().dispatch(new Action(ActionType::UploadFtpProcess, percentDone));
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		Dispatcher::instance().dispatch(new Action(ActionType::UploadFtpFinished));
	});
}
