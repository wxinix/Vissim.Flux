#ifndef CONSOLE_FTP_MIDDLEWARE_H
#define CONSOLE_FTP_MIDDLEWARE_H

#include <memory>
#include <vissim_flux.h>
#include <action/action_types.h>

class FtpMiddleware final : public vissim_flux::Middleware
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

#endif
