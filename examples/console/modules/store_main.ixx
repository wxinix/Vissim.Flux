module;
#include <any>
#include <iostream>
#include <memory>
#include <string>
export module store.main;

import action.types;
import vissim.flux;

export class MainStore final : public vissim_flux::Store 
{
public:
	MainStore() = default;
	MainStore(const MainStore&) = default;
	MainStore(MainStore&&) = default;
	MainStore& operator=(const MainStore&) = default;
	MainStore& operator=(MainStore&&) = default;
	~MainStore() = default;

	void process(const std::shared_ptr<vissim_flux::Action>& action) override;
};

void MainStore::process(const std::shared_ptr<vissim_flux::Action> &action)
{
    switch (action->getType<ActionType>()) {

        case ActionType::UploadFtpStarted:
            std::cout << "Start uploading " << action->getPayload<std::string>().c_str() << std::endl;
            break;

        case ActionType::UploadFtpProcess:
            std::cout << "Uploaded " << action->getPayload<int>() << "%" << std::endl;
            break;

        case ActionType::UploadFtpFinished:
            std::cout << "File uploaded successfully" << std::endl;
            break;
    }
}
