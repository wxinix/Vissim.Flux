#include <iostream>

#include <action/action_types.h>
#include <middleware/middleware_ftp.h>
#include <store/store_main.h>
#include <wxlib/flux.h>

using namespace wxlib::flux;

auto main() -> int {
	Dispatcher::instance().registerMiddleware(new FtpMiddleware);
	Dispatcher::instance().registerStore(new MainStore);
	Dispatcher::instance().dispatch(new Action(ActionType::UploadFtp, std::string("ftp://example.com/example.zip")));

	std::cin.get();
    return 0;
}
