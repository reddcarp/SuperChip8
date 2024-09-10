#include "schip8_emulator_vm.hpp"

#include <csignal>
#include <cxxopts.hpp>
#include <iostream>
#include <raylib.h>

// here to be able to properly close the VM on signal
SuperChip8::Emulator::VM *g_vm = nullptr;

int main(int argc, char *argv[]) {
  cxxopts::Options options("SuperChip8", "SuperChip8 Emulator");

  // clang-format off
  options.add_options()
  ("h,help", "Print help")
  ("r,rom", "Path to the ROM file", cxxopts::value<std::string>())
  ("c, cpu", "CPU cycles per frame - [Slow 5] | [Normal 10] | [Fast 100]", cxxopts::value<std::uint16_t>()->default_value("10"));
  // clang-format on

  // arg parsing
  auto result = options.parse(argc, argv);
  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return 0;
  }
  if (!result.count("rom")) {
    std::cerr << "Error: ROM file not provided" << std::endl;
    std::cout << options.help() << std::endl;
    return 1;
  }

  // signal handling
  std::signal(SIGINT, [](int) {
    if (g_vm) {
      g_vm->turnOff();
    }
    exit(0);
  });
  std::signal(SIGTERM, [](int) {
    if (g_vm) {
      g_vm->turnOff();
    }
    exit(0);
  });

  SuperChip8::Emulator::VM vm(result["cpu"].as<std::uint16_t>());
  g_vm = &vm;
  std::error_code ec;
  vm.turnOn(result["rom"].as<std::string>(), ec);
  if (ec) {
    std::cerr << "Error: " << ec.message() << std::endl;
    return 1;
  }

  vm.turnOff();

  return 0;
}