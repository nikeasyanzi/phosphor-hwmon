#include "gpio_handle.hpp"

#include <gpioplus/chip.hpp>
#include <gpioplus/handle.hpp>
#include <phosphor-logging/lg2.hpp>
#include <phosphor-logging/log.hpp>

#include <cstdlib>
#include <memory>
#include <string>

namespace gpio
{

using namespace phosphor::logging;

std::unique_ptr<gpioplus::HandleInterface> BuildGpioHandle(
    const std::string& gpiochip, const std::string& line)
{
    char *gpioEnd, *lineEnd;
    unsigned long chipId = std::strtoul(gpiochip.c_str(), &gpioEnd, 10);
    unsigned long lineOffset = std::strtoul(line.c_str(), &lineEnd, 10);

    if (!gpioEnd || gpioEnd != &gpiochip.c_str()[gpiochip.length()])
    {
        lg2::error("Unable to handle giochip entry {GPIOCHIP}", "GPIOCHIP",
                   gpiochip.c_str());
        return nullptr;
    }

    if (!lineEnd || lineEnd != &line.c_str()[line.length()])
    {
        lg2::error("Unable to handle line entry {}", "LINE", line.c_str());
        return nullptr;
    }

    try
    {
        gpioplus::Chip chip(chipId);
        gpioplus::HandleFlags flags(chip.getLineInfo(lineOffset).flags);
        flags.output = true;
        std::vector<gpioplus::Handle::Line> lines = {
            {static_cast<uint32_t>(lineOffset), 0}};

        return std::make_unique<gpioplus::Handle>(chip, lines, flags,
                                                  "phosphor-hwmon");
    }
    catch (const std::exception& e)
    {
        lg2::error("Unable to set up GPIO handle {ERROR}", "ERROR", e.what());
        return nullptr;
    }
}

} // namespace gpio
