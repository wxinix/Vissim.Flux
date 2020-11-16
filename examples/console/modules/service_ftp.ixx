module;
#include <chrono>
#include <future>
#include <queue>
#include <string>
#include <thread>
export module service.ftp;

import action.types;
import vissim.flux;

export class FtpService final
{
public:
	FtpService() = default;
	~FtpService() = default;

	void onUploadFtp(std::string&& filename);
private:
	FtpService(const FtpService&) = delete;
	FtpService(FtpService&&) = delete;
	FtpService& operator=(const FtpService&) = delete;
	FtpService& operator=(FtpService&&) = delete;

	std::future<void> future_;
};

void FtpService::onUploadFtp(std::string &&filename)
{
    future_ = std::async(std::launch::async, [] () {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        int percentDone = 0;
        for (int i = 0; i < 10; ++i) {
            percentDone += 10;
            vissim_flux::Dispatcher::instance().dispatch(new vissim_flux::Action(ActionType::UploadFtpProcess, percentDone));

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        vissim_flux::Dispatcher::instance().dispatch(new vissim_flux::Action(ActionType::UploadFtpFinished));
    });
}

