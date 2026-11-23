#include "vcamera/version.h"
#include <iostream>

int main()
{

	std::cout << "SDK 版本: " << vcamera::GetVersionString() << std::endl;
}