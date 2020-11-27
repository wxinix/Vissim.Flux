#ifndef CONSOLE_MAIN_STORE_H
#define CONSOLE_MAIN_STORE_H

#include <wxlib/flux.h>

class MainStore final: public wxlib::flux::Store
{
public:
	MainStore() = default;
	MainStore(const MainStore&) = default;
	MainStore(MainStore&&) = default;
	MainStore& operator=(const MainStore&) = default;
	MainStore& operator=(MainStore&&) = default;
	~MainStore() = default;

	void process(const std::shared_ptr<wxlib::flux::Action>& action) override;
};

#endif
