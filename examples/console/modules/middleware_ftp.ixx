module;
#include <any>
#include <memory>
#include <string>
export module middleware.ftp;

import action.types;
import service.ftp;
import vissim.flux;

export class FtpMiddleware final : public vissim_flux::Middleware
{
public:
	FtpMiddleware();
	~FtpMiddleware();

	std::shared_ptr<vissim_flux::Action> process(const std::shared_ptr<vissim_flux::Action>& action) override;
private:
	FtpMiddleware(const FtpMiddleware&) = delete;
	FtpMiddleware(FtpMiddleware&&) = delete;
	FtpMiddleware& operator=(const FtpMiddleware&) = delete;
	FtpMiddleware& operator=(FtpMiddleware&&) = delete;

	class FtpMiddlewareImpl;
	std::unique_ptr<FtpMiddlewareImpl> impl_;
};

class FtpMiddleware::FtpMiddlewareImpl final
{
public:
    FtpMiddlewareImpl()
        : service_(new FtpService)
    {
    }

    FtpMiddlewareImpl(const FtpMiddlewareImpl &) = delete;
    FtpMiddlewareImpl(FtpMiddlewareImpl &&) = delete;
    FtpMiddlewareImpl &operator=(const FtpMiddlewareImpl &) = delete;
    FtpMiddlewareImpl &operator=(FtpMiddlewareImpl &&) = delete;
    ~FtpMiddlewareImpl() = default;

    std::unique_ptr<FtpService> service_;
};

FtpMiddleware::FtpMiddleware()
    : impl_(new FtpMiddlewareImpl)
{
}

FtpMiddleware::~FtpMiddleware()
{
}

std::shared_ptr<vissim_flux::Action> FtpMiddleware::process(const std::shared_ptr<vissim_flux::Action> &action)
{
    switch (action->getType<ActionType>()) {
        case ActionType::UploadFtp:
            impl_->service_->onUploadFtp(action->getPayload<std::string>());
            return std::make_shared<vissim_flux::Action>(ActionType::UploadFtpStarted, action->getPayload<std::string>());
        default:
            return action;
    }
}

