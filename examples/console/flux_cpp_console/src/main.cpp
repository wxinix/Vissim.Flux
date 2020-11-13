#include <iostream>

#include <vissim_flux.h>

#include <action/action_types.h>
#include <middleware/ftp_middleware.h>
#include <store/main_store.h>

using namespace vissim_flux;

int main()
{
	Dispatcher::instance().registerMiddleware(new FtpMiddleware);
	Dispatcher::instance().registerStore(new MainStore);

	Dispatcher::instance().dispatch(new Action(ActionType::UploadFtp, std::string("ftp://example.com/example.zip")));

	std::cin.get();
    return 0;
}
