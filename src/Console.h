#pragma once

#include <xstring>

namespace Console {
    void setTitle(std::string_view title);
	void setSize(short x, short y);
	void setFlags(long flags);
};
