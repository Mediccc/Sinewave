#pragma once
#include <iostream>
#include <fstream>
#include <imgui.h>
#include <vector>
#include <set>
#include "../misc.h"
#include "imgui_stdlib.h"
#include "bun/bunui.h"

extern std::string fflag;
extern std::string fflagValue;

void checkFFlags();
void setFflag(const std::string& fflag, const std::string& value);

struct FFlag {
    std::string name;
    std::string value;
};

extern std::vector<FFlag> flags;

void initFflags();