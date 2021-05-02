#include "first_app.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {
	ae::FirstApp app{};

	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}