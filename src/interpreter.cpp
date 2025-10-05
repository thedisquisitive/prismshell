#include "prismshell/interpreter.hpp"
#include "prismshell/runtime.hpp"
#include "prismshell/utils.hpp"

#include <atomic>
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <chrono>
#include <ctime>

#ifdef USE_READLINE
extern "C" {
#include <readline/history.h>
#include <readline/readline.h>
extern int rl_catch_signals;
}
#endif
#include <algorithm>

namespace fs = std::filesystem;

namespace pb {

// ---- externs provided by runtime.cpp (mod system) ----
bool mod_has(const std::string& name);
int  mod_run(const std::string& name, const std::vector<std::string>& args, Runtime& parent);
int  mod_run_capture(const std::string& name, const std::vector<std::string>& args, Runtime& parent, std::string* out);
static Value num(double d) { return Value{d}; }
static Value str(std::string s) { return Value{std::move(s)}; }

// ---------------- SIGINT (Ctrl-C) handling ----------------
namespace {
  inline std::atomic_bool g_sigint{false};
  void on_sigint(int){ g_sigint.store(true, std::memory_order_relaxed); }
  void install_sig_handlers(){
    struct sigaction sa{}; sa.sa_handler = on_sigint; sigemptyset(&sa.sa_mask); sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
    std::signal(SIGQUIT, SIG_IGN);
    std::signal(SIGPIPE, SIG_IGN);
  }
  inline bool take_interrupt(){ return g_sigint.exchange(false, std::memory_order_relaxed); }
}

// ---------------- helpers ----------------
static bool is_integer_line(const std::string& s){
  size_t i = 0; while(i<s.size() && std::isspace((unsigned char)s[i])) ++i;
  if(i>=s.size() || !std::isdigit((unsigned char)s[i])) return false;
  while(i<s.size() && std::isdigit((unsigned char)s[i])) ++i;
  return true;
}

static std::string to_upper(std::string s){ for(char& c: s) c=(char)std::toupper((unsigned char)c); return s; }

static std::string json_escape(const std::string& in){
  std::string out; out.reserve(in.size()+8);
  for(unsigned char c: in){
    switch(c){
      case '\\': out += "\\\\"; break;
      case '"' : out += "\\\""; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if(c < 0x20){ char b[8]; std::snprintf(b,sizeof(b),"\\u%04x", c); out += b; }
        else out += (char)c;
    }
  }
  return out;
}
static std::string json_array(const std::vector<std::string>& xs){
  std::ostringstream oss; oss << "[";
  for(size_t i=0;i<xs.size();++i){ if(i) oss << ","; oss << "\"" << json_escape(xs[i]) << "\""; }
  oss << "]"; return oss.str();
}

// shell-like tokenizer with quotes/escapes
static std::vector<std::string> tokenize_quoted(const std::string& line){
  std::vector<std::string> out; std::string cur;
  bool in_single=false, in_double=false, escaping=false;
  auto push = [&](){ out.push_back(cur); cur.clear(); };
  for(size_t i=0;i<line.size();++i){
    char c=line[i];
    if(escaping){ cur.push_back(c); escaping=false; continue; }
    if(c=='\\'){ if(in_single) cur.push_back(c); else escaping=true; continue; }
    if(c=='\'' && !in_double){ in_single=!in_single; continue; }
    if(c=='"' && !in_single){ in_double=!in_double; continue; }
    if(std::isspace((unsigned char)c) && !in_single && !in_double){
      if(!cur.empty()) push();
      while(i+1<line.size() && std::isspace((unsigned char)line[i+1])) ++i;
      continue;
    }
    cur.push_back(c);
  }
  if(!cur.empty() || (!in_single && !in_double && out.empty())) push();
  return out;
}

// ---------------- PrismRC: ~/.prismrc ----------------
namespace {
  struct PrismRC {
    std::string mods_path = "mods";         // reserved for future search path override
    bool autoload_all = false;              // if true, autostart every discovered mod
    std::vector<std::string> autostart;     // filenames to autostart (order preserved)
  };

  static inline std::string pb_rc_ltrim(std::string s){
    size_t i = 0;
    while (i < s.size() && std::isspace((unsigned char)s[i])) ++i;
    return s.substr(i);
  }
  static inline std::string pb_rc_rtrim(std::string s){
    if (s.empty()) { return s; }
    size_t i = s.size();
    while (i > 0 && std::isspace((unsigned char)s[i-1])) { --i; }
    return s.substr(0, i);
  }
  static inline std::string pb_rc_trim(std::string s){
    return pb_rc_rtrim(pb_rc_ltrim(std::move(s)));
  }

  static std::vector<std::string> pb_rc_splitWordsOrCSV(const std::string& v){
    std::vector<std::string> out; std::string cur; bool inq=false; char q=0;
    for(char c: v){
      if(inq){ if(c==q) inq=false; else cur.push_back(c); continue; }
      if(c=='"'||c=='\''){ inq=true; q=c; continue; }
      if(std::isspace((unsigned char)c) || c==','){ if(!cur.empty()){ out.push_back(pb_rc_trim(cur)); cur.clear(); } continue; }
      cur.push_back(c);
    }
    if(!cur.empty()) out.push_back(pb_rc_trim(cur));
    std::vector<std::string> out2; for(auto&s: out){ auto t=pb_rc_trim(s); if(!t.empty()) out2.push_back(std::move(t)); }
    return out2;
  }

  static std::string pb_rc_home(){
  #ifdef _WIN32
    if(const char* p=std::getenv("PRISMRC_HOME")) return p;
    if(const char* p=std::getenv("USERPROFILE"))  return p;
    if(const char* d=std::getenv("HOMEDRIVE")){
      std::string hd=d; std::string hp=std::getenv("HOMEPATH")?std::getenv("HOMEPATH"):""; return hd+hp;
    }
    return ".";
  #else
    if(const char* p=std::getenv("PRISMRC_HOME")) return p;
    if(const char* p=std::getenv("HOME"))         return p;
    return ".";
  #endif
  }

  static PrismRC loadPrismRC(){
    PrismRC rc;
    std::string rcPath;
    if(const char* p = std::getenv("PRISMRC")) rcPath = p;
    if(rcPath.empty()) rcPath = (std::filesystem::path(pb_rc_home()) / ".prismrc").string();

    std::ifstream f(rcPath);
    if(!f){ rc.autostart = {"prompt.bas"}; return rc; }

    std::string line;
    while(std::getline(f,line)){
      auto h=line.find('#'); if(h!=std::string::npos) line.erase(h);
      auto s=line.find(';'); if(s!=std::string::npos) line.erase(s);
      line = pb_rc_trim(std::move(line)); if(line.empty()) continue;

      auto eq=line.find('='); if(eq==std::string::npos) continue;
      std::string key=pb_rc_trim(line.substr(0,eq)), val=pb_rc_trim(line.substr(eq+1));
      for(char& c: key) c=(char)std::tolower((unsigned char)c);

      if(key=="mods_path"   && !val.empty()) rc.mods_path = val;          // (not used yet)
      else if(key=="autoload_all") rc.autoload_all = (val=="1"||val=="true"||val=="yes"||val=="on");
      else if(key=="autostart")    rc.autostart = pb_rc_splitWordsOrCSV(val);
    }
    if(!rc.autoload_all && rc.autostart.empty()) rc.autostart = {"prompt.bas"};
    return rc;
  }
} // namespace

// query runtime for registered mods via CALL Mod.List()
static std::vector<std::string> list_mod_names_via_call(Runtime& rt){
  (void)rt.run_line_direct("CALL Mod.List()", 0);
  std::string names = to_string(rt.lastCall);
  std::vector<std::string> out; std::string line;
  std::istringstream iss(names);
  while(std::getline(iss, line)){ line = trim(line); if(!line.empty()) out.push_back(line); }
  return out;
}

// ---- MOD AUTOLOAD ---------------------------------------------------------
static bool has_ext_ci(const fs::path& p, const std::vector<std::string>& exts){
  auto e = p.extension().string();
  std::string up; up.reserve(e.size());
  for(char c: e) up.push_back((char)std::toupper((unsigned char)c));
  for(const auto& want : exts){
    if(up == want) return true;
  }
  return false;
}

static void load_mod_file(Runtime& rt, const fs::path& p){
  std::ifstream f(p);
  if(!f) return;
  std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
  // strip shebang if present
  if(!content.empty() && content.rfind("#!", 0) == 0){
    auto nl = content.find('\n');
    content = (nl == std::string::npos) ? std::string() : content.substr(nl+1);
  }
  // numbered?
  bool numbered = true;
  {
    std::istringstream iss(content); std::string line;
    while(std::getline(iss, line)){
      std::string t = trim(line); if(t.empty()) continue;
      if(!std::isdigit((unsigned char)t[0])){ numbered=false; break; }
    }
  }
  Runtime mrt; // child runtime to execute registration code
  if(numbered){
    std::istringstream iss2(content); std::string line;
    while(std::getline(iss2, line)){
      std::string t = trim(line); if(t.empty()) continue;
      auto sp = t.find(' '); if(sp == std::string::npos) continue;
      int n = std::stoi(t.substr(0, sp));
      std::string src = trim(t.substr(sp+1));
      mrt.program[n] = src;
    }
  } else {
    std::istringstream iss2(content); std::string line; int n=10;
    while(std::getline(iss2, line)){
      std::string t = trim(line); if(t.empty()) continue;
      mrt.program[n] = t; n += 10;
    }
  }
  mrt.vars["PB_ARGV"] = std::string("[]");
  int entry = mrt.program.empty() ? 0 : mrt.program.begin()->first;
  std::string name = p.stem().string();
  auto esc = [](const std::string& s){ std::string o; o.reserve(s.size()+4); for(char c: s){ if(c=='"') o += "\\\""; else o.push_back(c); } return o; };
  (void)mrt.run_line_direct(std::string("CALL Mod.Register(\"")+esc(name)+"\", "+std::to_string(entry)+")", 0);
}

static std::vector<fs::path> mod_search_paths(){
  std::vector<fs::path> roots;
  // project-local (current dir)
  roots.emplace_back(fs::path("mods"));
  // user
  if(const char* home = std::getenv("HOME")){
    roots.emplace_back(fs::path(home) / ".config/prismshell/mods");
  }
  // system
  roots.emplace_back(fs::path("/usr/local/share/prismshell/mods"));
  roots.emplace_back(fs::path("/usr/share/prismshell/mods"));
  // env PRISMSHELL_MOD_PATH (colon-separated)
  if(const char* extra = std::getenv("PRISMSHELL_MOD_PATH")){
    std::string s = extra;
    size_t start=0;
    while(true){
      size_t sep = s.find(':', start);
      std::string chunk = (sep==std::string::npos) ? s.substr(start) : s.substr(start, sep-start);
      if(!chunk.empty()) roots.emplace_back(chunk);
      if(sep==std::string::npos) break;
      start = sep+1;
    }
  }
  return roots;
}

static void autoload_mods(Runtime& rt){
  // Try to clear registry if supported
  (void)rt.run_line_direct("CALL Mod.Clear()", 0); // ignore errors if not implemented

  std::vector<std::string> exts = { ".BAS", ".PBAS" };
  for(const auto& root : mod_search_paths()){
    std::error_code ec;
    if(!fs::exists(root, ec) || !fs::is_directory(root, ec)) continue;
    for(auto it = fs::directory_iterator(root, ec); !ec && it!=fs::end(it); it.increment(ec)){
      if(!it->is_regular_file()) continue;
      const auto& p = it->path();
      if(has_ext_ci(p, exts)){
        load_mod_file(rt, p);
      }
    }
  }
}

// prompt building bits
static std::string now_time_hhmmss(){
  using namespace std::chrono;
  auto t = system_clock::to_time_t(system_clock::now());
  std::tm tm{}; localtime_r(&t, &tm);
  char buf[16]; std::snprintf(buf,sizeof(buf), "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
  return buf;
}
static std::string short_cwd(const std::string& full){
  if(full.empty()) return full;

  // Split path
  const char sep = '/';
  std::vector<std::string> parts;
  size_t pos=0, prev=0;
  while((pos = full.find(sep, prev)) != std::string::npos){
    if(pos > prev) parts.push_back(full.substr(prev, pos - prev));
    prev = pos + 1;
  }
  if(prev < full.size()) parts.push_back(full.substr(prev));

  // If it's the root
  if(parts.empty()) return "/";

  // Keep last two segments verbatim when depth >= 3, otherwise compact everything.
  const size_t keep_last = 2;
  const bool shallow = parts.size() <= keep_last;

  std::ostringstream out;
  out << sep;
  for(size_t i = 0; i < parts.size(); ++i){
    bool keep_full = (!shallow && i >= parts.size() - keep_last);
    if(keep_full) {
      out << parts[i];
    } else {
      out << (parts[i].empty() ? "" : std::string(1, parts[i][0]));
    }
    if(i + 1 < parts.size()) out << sep;
  }
  return out.str();
}

static std::string expand_template(std::string tmpl, int last_status){
  auto rep=[&](const std::string& k,const std::string& v){
    std::string tag="${"+k+"}"; size_t p=0;
    while((p=tmpl.find(tag,p))!=std::string::npos){ tmpl.replace(p,tag.size(),v); p+=v.size(); }
  };
  std::string cwd = fs::current_path().string();
  rep("cwd", cwd);
  rep("shortcwd", short_cwd(cwd));
  rep("status", std::to_string(last_status));
  rep("status_emoji", last_status==0 ? "✅" : "❌");
  rep("time", now_time_hhmmss());
  return tmpl;
}
static std::string build_prompt(Runtime& rt, int last_status, const std::unordered_set<std::string>& disabled){
  // 1) prompt mod
  if(!disabled.count("prompt") && mod_has("prompt")){
    std::string out;
    int rc = mod_run_capture("prompt", {"--status", std::to_string(last_status)}, rt, &out);
    if(rc==0 && !out.empty()) return out;
  }
  // 2) template
  auto it = rt.vars.find("PB_PROMPT_TMPL");
  if(it!=rt.vars.end()){
    return expand_template(to_string(it->second), last_status);
  }
  // 3) default
  return expand_template("${status_emoji} ${shortcwd} pbsh> ", last_status);
}

// ---- builtins: cd & pwd (affect parent process) ----
static int builtin_cd(const std::vector<std::string>& argv){
  std::string target;
  if (argv.size() <= 1 || argv[1].empty()) {
    const char* home = std::getenv("HOME");
    target = home ? home : "/";
  } else if (argv[1] == "-") {
    const char* oldpwd = std::getenv("OLDPWD");
    if (!oldpwd) { std::cerr << "cd: OLDPWD not set\n"; return 1; }
    target = oldpwd;
    std::cout << target << "\n"; // bash prints path on 'cd -'
  } else {
    target = argv[1];
  }

  // expand leading ~
  if (!target.empty() && target[0] == '~') {
    const char* home = std::getenv("HOME");
    if (home) target = std::string(home) + target.substr(1);
  }

  std::error_code ec;
  fs::path newp = fs::path(target);
  if (newp.is_relative()) newp = fs::current_path(ec) / newp;
  fs::path canon = fs::weakly_canonical(newp, ec);
  if (!ec && !canon.empty()) newp = canon;
  std::string prev = fs::current_path().string();

  if (!fs::exists(newp) || !fs::is_directory(newp)) {
    std::cerr << "cd: no such directory: " << target << "\n";
    return 1;
  }
  fs::current_path(newp, ec);
  if (ec) { std::cerr << "cd: " << ec.message() << "\n"; return 1; }

#ifndef _WIN32
  setenv("OLDPWD", prev.c_str(), 1);
  setenv("PWD", newp.string().c_str(), 1);
#else
  _putenv_s("OLDPWD", prev.c_str());
  _putenv_s("PWD", newp.string().c_str());
#endif
  return 0;
}

static int builtin_pwd(){
  std::error_code ec;
  auto p = fs::current_path(ec);
  if (ec) { std::cerr << "pwd: " << ec.message() << "\n"; return 1; }
  std::cout << p.string() << "\n";
  return 0;
}

// mods meta helpers
static void print_mods(Runtime& rt, const std::unordered_set<std::string>& disabled){
  auto names = list_mod_names_via_call(rt);
  if(names.empty()){ std::cout << "(no mods loaded)\n"; return; }
  for(auto& n: names) std::cout << n << (disabled.count(n) ? " [disabled]\n" : "\n");
}
static void mods_help(){
  std::cout <<
  "mods commands:\n"
  "  mods                 # list mods\n"
  "  mods list            # list mods\n"
  "  mods enable <name>   # enable a mod\n"
  "  mods disable <name>  # disable a mod\n"
  "  mods reload          # rescan mod directories\n"
  "  mods run <name> [args...]  # run a mod\n";
}

// disabled mods set (session)
static std::unordered_set<std::string> g_disabled_mods;

// ---------------- Interpreter ----------------
void Interpreter::repl(const char* /*prompt_ignored*/){
  install_sig_handlers();
#ifdef USE_READLINE
  rl_catch_signals = 0; // we handle SIGINT
#endif

  int last_status = 0;
  std::cout << "PrismBASIC Shell — MVP (type HELP)\n";


// Discover & register all mods, then autostart per ~/.prismrc
PrismRC rc = loadPrismRC();
autoload_mods(rt);
std::unordered_set<std::string> available;
for (auto& n : list_mod_names_via_call(rt)) available.insert(n);
std::vector<std::string> toRun;
if (rc.autoload_all) {
  toRun.assign(available.begin(), available.end());
  std::sort(toRun.begin(), toRun.end());
} else {
  for (const auto& fname : rc.autostart) {
    if (fname.empty()) continue;
    std::string name = fname;
    auto dot = name.find_last_of('.');
    if (dot != std::string::npos) name = name.substr(0, dot);
    if (available.count(name)) toRun.push_back(name);
    else std::cerr << "[prismshell] warning: autostart mod not found: " << name << "\n";
  }
}
for (const auto& m : toRun) {
  if (g_disabled_mods.count(m)) continue;
  (void)mod_run(m, {}, rt);
}

  while(true){
    std::string line;
#ifndef USE_READLINE
    std::string ps = build_prompt(rt, last_status, g_disabled_mods);
    std::cout << ps;
    if(!std::getline(std::cin, line)){
      if(take_interrupt()){ std::cout << "\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }
      break;
    }
#else
    std::string ps = build_prompt(rt, last_status, g_disabled_mods);
    char* in = readline(ps.c_str());
    if(!in){
      if(take_interrupt()){ std::cout << "\n"; continue; }
      break;
    }
    line.assign(in);
    if(!line.empty()) add_history(in);
    free(in);
#endif

    std::string s = trim(line);
    if(s.empty()){ last_status = 0; continue; }

    // Meta commands
    std::string up = to_upper(s);
    if(up=="HELP"){
      std::cout << "Commands: LIST, RUN, NEW, SAVE <file>, LOAD <file>, BYE\n";
      std::cout << "BASIC: line-numbered edits; PRINT/LET/INPUT/IF...THEN/GOTO/GOSUB/RETURN/CALL/END\n";
      std::cout << "Builtins: cd, pwd\n";
      std::cout << "MODS: type 'mods' for mod management\n";
      last_status = 0; continue;
    }
    if(up=="BYE" || up=="EXIT") break;
    if(up=="LIST"){ rt.list(); last_status=0; continue; }
    if(starts_with(up, "SAVE ")){ std::string p=trim(s.substr(5)); last_status = rt.save(p)?0:1; if(last_status) std::cout<<"Save failed\n"; continue; }
    if(starts_with(up, "LOAD ")){ std::string p=trim(s.substr(5)); last_status = rt.load(p)?0:1; if(last_status) std::cout<<"Load failed\n"; continue; }
    if(up=="NEW"){ rt.program.clear(); last_status=0; continue; }
    if(up=="RUN"){ auto r=rt.run_program(); last_status = r.err?1:0; if(r.err) std::cout<<"Error at "<<r.err->line<<": "<<r.err->msg<<"\n"; continue; }

    // mods meta
    if(starts_with(up, "MODS")){
      auto argv = tokenize_quoted(s);
      std::string sub = (argv.size()>=2 ? to_upper(argv[1]) : "LIST");
      if(sub=="LIST" || argv.size()==1){ print_mods(rt, g_disabled_mods); last_status=0; continue; }
      if(sub=="HELP"){ mods_help(); last_status=0; continue; }
      if(sub=="RELOAD"){
        autoload_mods(rt);
        std::cout<<"mods reloaded\n"; last_status=0; continue;
      }
      if(sub=="ENABLE" && argv.size()>=3){ g_disabled_mods.erase(argv[2]); std::cout<<"enabled "<<argv[2]<<"\n"; last_status=0; continue; }
      if(sub=="DISABLE" && argv.size()>=3){ g_disabled_mods.insert(argv[2]); std::cout<<"disabled "<<argv[2]<<"\n"; last_status=0; continue; }
      if(sub=="RUN" && argv.size()>=3){
        std::string name = argv[2]; std::vector<std::string> a; for(size_t i=3;i<argv.size();++i) a.push_back(argv[i]);
        if(g_disabled_mods.count(name)){ std::cout<<"mod '"<<name<<"' is disabled\n"; last_status=1; continue; }
        if(!mod_has(name)){ std::cout<<"no such mod: "<<name<<"\n"; last_status=127; continue; }
        last_status = mod_run(name, a, rt); continue;
      }
      std::cout<<"unknown mods command; try 'mods help'\n"; last_status=2; continue;
    }

    // Builtins: cd/pwd (before BASIC/mod/shell)
    {
      auto argv = tokenize_quoted(s);
      if(!argv.empty()){
        if(argv[0] == "cd"){
          last_status = builtin_cd(argv);
          continue;
        }
        if(argv[0] == "pwd"){
          last_status = builtin_pwd();
          continue;
        }
      }
    }

    // Retro line-numbered edit
    if(is_integer_line(s)){
      std::istringstream iss(s); int n=0; iss>>n; std::string rest; std::getline(iss,rest); rest=trim(rest);
      if(rest.empty()) rt.program.erase(n); else rt.program[n]=rest;
      last_status = 0; continue;
    }

    // Try BASIC direct
    if(auto r = rt.run_line_direct(s, 0); !r.err){ last_status = 0; continue; }

    // Try mod dispatch first (quoted args aware)
    {
      auto argv = tokenize_quoted(s);
      if(!argv.empty()){
        std::string cmd = argv[0]; std::vector<std::string> a; for(size_t i=1;i<argv.size();++i) a.push_back(argv[i]);
        if(!cmd.empty() && !g_disabled_mods.count(cmd) && mod_has(cmd)){
          last_status = mod_run(cmd, a, rt);
          continue;
        }
      }
    }

    // Fallback: /bin/sh -lc  (parent ignores SIGINT so Ctrl-C kills only child)
    auto prev = std::signal(SIGINT, SIG_IGN);
    last_status = rt.sh_exec(s);
    std::signal(SIGINT, prev);
    (void)take_interrupt(); // drain pending SIGINT so next prompt isn't interrupted
  }
}

  int Interpreter::run_file(const std::string& path, const std::vector<std::string>& args){
    std::ifstream f(path);
    if(!f){
      std::cerr << "No such file: " << path << "\n";
      return 1;
    }

    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    // Strip shebang if present
    if(!content.empty() && content.rfind("#!", 0) == 0){
      auto nl = content.find('\n');
      content = (nl == std::string::npos) ? std::string() : content.substr(nl + 1);
    }

    // UPDATED: Pass argv to program in multiple formats
    rt.vars["PB_ARGV"] = json_array(args);  // Legacy compatibility
    rt.vars["PB_ARGC"] = num((double)args.size());
    
    // Individual PB_ARG0, PB_ARG1, etc.
    for (size_t i = 0; i < args.size(); ++i) {
      std::string key = "PB_ARG" + std::to_string(i);
      rt.vars[key] = str(args[i]);
    }
    
    // NEW: Create ARGV array for direct access
    ArrayData argv_array;
    argv_array.dimensions = {args.size()};
    argv_array.isDynamic = false;
    argv_array.data.resize(args.size());
    for (size_t i = 0; i < args.size(); ++i) {
      argv_array.data[i] = str(args[i]);
    }
    rt.arrays["ARGV"] = argv_array;

    // Helper: detect BASIC comment line after trimming
    auto is_comment_line = [](const std::string& t)->bool{
      if(t.empty()) return false;
      if(t[0] == '\'') return true;                        // apostrophe comment
      if(t.size() >= 3){
        unsigned char c0 = (unsigned char)t[0];
        unsigned char c1 = (unsigned char)t[1];
        unsigned char c2 = (unsigned char)t[2];
        if(std::toupper(c0)=='R' && std::toupper(c1)=='E' && std::toupper(c2)=='M')
          return true;                                     // REM comment
      }
      return false;
    };

    // Decide: numbered vs free-form (skip blanks & comments)
    std::istringstream iss(content);
    std::string line;
    bool numbered = true;
    while(std::getline(iss, line)){
      std::string t = trim(line);
      if(t.empty()) continue;
      if(is_comment_line(t)) continue;
      numbered = std::isdigit((unsigned char)t[0]) != 0;
      break;
    }

    // Build program map
    rt.program.clear();

    if(numbered){
      // Parse explicit line numbers
      std::istringstream iss2(content);
      while(std::getline(iss2, line)){
        std::string t = trim(line);
        if(t.empty()) continue;

        // Expect: "<digits><space>rest..."
        auto sp = t.find(' ');
        if(sp == std::string::npos) continue;

        const std::string num = t.substr(0, sp);
        bool all_digits = !num.empty() &&
          std::all_of(num.begin(), num.end(), [](unsigned char c){ return std::isdigit(c); });
        if(!all_digits) continue;

        int n = 0;
        try { n = std::stoi(num); }
        catch (...) { continue; }

        std::string src = trim(t.substr(sp + 1));
        rt.program[n] = src;
      }
    }else{
      // Auto-number free-form lines (skip blanks & comments)
      std::istringstream iss2(content);
      int n = 10;
      while(std::getline(iss2, line)){
        std::string t = trim(line);
        if(t.empty()) continue;
        if(is_comment_line(t)) continue;      // don't inject banner/comments as code
        rt.program[n] = t;
        n += 10;
      }
    }
    extract_subs(rt);
    auto r = rt.run_program();
    if(r.err){
      std::cerr << "Error at " << r.err->line << ": " << r.err->msg << "\n";
      return 2;
    }
    return 0;
  }


} // namespace pb
