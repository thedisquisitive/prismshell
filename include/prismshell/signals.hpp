#pragma once
#include <atomic>
#include <csignal>

namespace pb {

// global interrupt flag visible across TU's
inline std::atomic_bool g_sigint{false};

inline void install_basic_signal_handlers() {
  // SIGINT: set flag; don't terminate the process
  struct sigaction sa{};
  sa.sa_handler = [](int){ g_sigint.store(true, std::memory_order_relaxed); };
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0; // not SA_RESTART: we want reads to interrupt
  sigaction(SIGINT, &sa, nullptr);

  // Nice-to-haves to avoid unpleasant exits
  std::signal(SIGQUIT, SIG_IGN); // ignore Ctrl-\
  std::signal(SIGPIPE, SIG_IGN); // avoid SIGPIPE killing us
}

inline bool take_interrupt() {
  // returns true once per delivered SIGINT
  return g_sigint.exchange(false, std::memory_order_relaxed);
}

} // namespace pb
