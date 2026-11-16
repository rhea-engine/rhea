#pragma once

#include <string>
#include <vector>

enum class BrowserPlaceKind { Internal, Location };

struct BrowserPlace
{
	std::string name;
	std::string address;
	BrowserPlaceKind kind;
};

struct BrowserData
{
	std::vector<BrowserPlace> places;
	std::vector<std::string> recent_locations;
};