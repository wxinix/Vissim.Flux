#include <string>

#include <wxlib/flux.h>
#include <action/action_types.h>
#include <middleware/middleware_ftp.h>
#include <service/service_ftp.h>

class FtpMiddleware::FtpMiddlewareImpl final
{
public:
	FtpMiddlewareImpl(): service_(new FtpService)
	{
	}

	FtpMiddlewareImpl(const FtpMiddlewareImpl&) = delete;
	FtpMiddlewareImpl(FtpMiddlewareImpl&&) = delete;
	~FtpMiddlewareImpl() = default;

	FtpMiddlewareImpl& operator=(const FtpMiddlewareImpl&) = delete;
	FtpMiddlewareImpl& operator=(FtpMiddlewareImpl&&) = delete;	

	std::unique_ptr<FtpService> service_;
};

FtpMiddleware::FtpMiddleware(): impl_(new FtpMiddlewareImpl)
{
}

FtpMiddleware::~FtpMiddleware()
{
}

void FtpMiddleware::process(const std::shared_ptr<wxlib::flux::Action>& action)
{
	using wxlib::flux::Dispatcher;
	using wxlib::flux::Action;

	switch (action->getType<ActionType>()) {
        case ActionType::UploadFtp:
            impl_->service_->onUploadFtp(action->getPayload<std::string>());
            Dispatcher::instance().dispatch(new Action(ActionType::UploadFtpStarted, action->getPayload<std::string>()));
			break;
        default:
            return;
	}
}
