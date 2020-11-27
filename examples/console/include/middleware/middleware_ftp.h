#ifndef CONSOLE_FTP_MIDDLEWARE_H
#define CONSOLE_FTP_MIDDLEWARE_H

#include <memory>
#include <wxlib/flux.h>
#include <action/action_types.h>

class FtpMiddleware final: public wxlib::flux::Middleware
{
public:
	FtpMiddleware();
	~FtpMiddleware();

	void process(const std::shared_ptr<wxlib::flux::Action>& action) override;
private:
	FtpMiddleware(const FtpMiddleware&) = delete;
	FtpMiddleware(FtpMiddleware&&) = delete;
	
	FtpMiddleware& operator=(const FtpMiddleware&) = delete;
	FtpMiddleware& operator=(FtpMiddleware&&) = delete;

	class FtpMiddlewareImpl;
	std::unique_ptr<FtpMiddlewareImpl> impl_;
};

#endif
