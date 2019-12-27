#include <cstdlib>
#include <fstream>
#include <vector>

#include "CubeReader.h"
#include "DotReader.h"
#include "IPCGReader.h"

#include "Callgraph.h"

#include "EdgeBasedOptimumEstimatorPhase.h"
#include "ExtrapEstimatorPhase.h"
#include "IPCGEstimatorPhase.h"
#include "NodeBasedOptimumEstimatorPhase.h"
#include "ProximityMeasureEstimatorPhase.h"
#include "SanityCheckEstimatorPhase.h"

#include "cxxopts.hpp"

void registerEstimatorPhases(CallgraphManager &cg, Config *c, bool isIPCG, float runtimeThreshold) {
  auto statEstimator = new StatisticsEstimatorPhase(false);
  cg.registerEstimatorPhase(new RemoveUnrelatedNodesEstimatorPhase(true, false));  // remove unrelated
  cg.registerEstimatorPhase(new ResetEstimatorPhase());
  cg.registerEstimatorPhase(statEstimator);
  cg.registerEstimatorPhase(new ResetEstimatorPhase());

  // Actually do the selection
  if (!isIPCG) {
    std::cout << "New threshold runtime for profiling: " << runtimeThreshold << std::endl;
    cg.registerEstimatorPhase(new RuntimeEstimatorPhase(runtimeThreshold));
  } else {
    const int nStmt = 2000;
    std::cout << "[PGIS] [STATIC] $" << nStmt << "$" << std::endl;
    cg.registerEstimatorPhase(new StatementCountEstimatorPhase(nStmt, true, statEstimator));
  }

  cg.registerEstimatorPhase(new StatisticsEstimatorPhase(true));

  //  cg.registerEstimatorPhase(new ResetEstimatorPhase());
}

bool stringEndsWith(const std::string &s, const std::string &suffix) {
  return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

template <typename Target, typename OptsT, typename ConfigT>
void checkAndSet(const char *id, const OptsT &opts, ConfigT &cfg) {
  if (opts.count(id)) {
    cfg = opts[id].template as<Target>();
  }
}

int main(int argc, char **argv) {
  if (argc == 1) {
    std::cerr << "ERROR: too few arguments. Use --help to show help." << std::endl;
    exit(-1);
  }

  cxxopts::Options opts("PGIS", "Generating low-overhead instrumentation selections.");

  // clang-format off
  opts.add_options()
    ("other", "", cxxopts::value<std::string>()->default_value(""))
    ("s,samples", "Samples per second", cxxopts::value<long>()->default_value("0"))
    ("r,ref", "??", cxxopts::value<double>()->default_value("0"))
    ("m,mangled", "Use mangled names", cxxopts::value<bool>()->default_value("false"))
    ("half", "??", cxxopts::value<long>()->default_value("0"))
    ("t,tiny", "Print tiny report", cxxopts::value<bool>()->default_value("false"))
    ("i,ignore-sampling", "Ignore sampling", cxxopts::value<bool>()->default_value("false"))
    ("f,samples-file", "Input file for sampling points", cxxopts::value<std::string>())
    ("g,greedy-unwind", "Use greedy unwind", cxxopts::value<bool>()->default_value("false"))
    ("o,out-file", "Output file name", cxxopts::value<std::string>()->default_value("out"))
    ("static", "Apply static selection", cxxopts::value<bool>()->default_value("false"))
    ("c,cube", "Cube file for dynamic instrumentation", cxxopts::value<std::string>()->default_value(""))
    ("h,help", "Show help", cxxopts::value<bool>()->default_value("false"))
    ("e,extrap", "File to read Extra-P info from", cxxopts::value<std::string>()->default_value(""))
    ("model-filter", "Use Extra-P models to filter only.", cxxopts::value<bool>()->default_value("false"))
    ("a,all-threads","Show all Threads even if unused.", cxxopts::value<bool>()->default_value("false"))
    ("w, whitelist", "Filter nodes through given whitelist", cxxopts::value<std::string>()->default_value(""));
  // clang-format on

  Config c;
  bool applyStaticFilter = false;
  bool applyModelFilter = false;
  auto result = opts.parse(argc, argv);

  if (result.count("help")) {
    std::cout << opts.help() << "\n";
    return 0;
  }

  /* Required positional arguments */
  // opts.parse_positional("ipcg-file");

  /* Additional options */
  checkAndSet<std::string>("other", result, c.otherPath);
  checkAndSet<int>("samples", result, CgConfig::samplesPerSecond);
  checkAndSet<double>("ref", result, c.referenceRuntime);
  checkAndSet<bool>("mangled", result, c.useMangledNames);
  checkAndSet<int>("half", result, c.nanosPerHalfProbe);
  checkAndSet<bool>("tiny", result, c.tinyReport);
  checkAndSet<bool>("ignore-sampling", result, c.ignoreSamplingOv);
  checkAndSet<std::string>("samples-file", result, c.samplesFile);
  checkAndSet<bool>("greedy-unwind", result, c.greedyUnwind);
  checkAndSet<std::string>("out-file", result, c.outputFile);
  checkAndSet<bool>("static", result, applyStaticFilter);
  checkAndSet<bool>("model-filter", result, applyModelFilter);
  checkAndSet<bool>("all-threads", result, c.showAllThreads);
  checkAndSet<std::string>("whitelist", result, c.whitelist);
  // for static instrumentation
  std::string ipcgFullPath(argv[argc - 1]);
  // checkAndSet<std::string>("ipcg-file", result, ipcgFullPath);
  std::string ipcgFilename = ipcgFullPath.substr(ipcgFullPath.find_last_of('/') + 1);
  c.appName = ipcgFilename.substr(0, ipcgFilename.find_last_of('.'));

  const auto parseExtrapArgs = [](auto argsRes) {
    if (argsRes.count("extrap")) {
      // Read in extra-p configuration
      std::string filePath(argsRes["extrap"].template as<std::string>());
      std::ifstream epFile(filePath);
      return extrapconnection::getExtrapConfigFromJSON(filePath);
    } else {
      return extrapconnection::ExtrapConfig();
    }
  };

  float runTimeThreshold = .0f;
  CallgraphManager cg(&c, parseExtrapArgs(result));

  if (stringEndsWith(ipcgFullPath, ".ipcg")) {
    std::cout << "Reading from ipcg file " << ipcgFullPath << std::endl;
    IPCGAnal::buildFromJSON(cg, ipcgFullPath, &c);
    if (applyStaticFilter) {
      registerEstimatorPhases(cg, &c, true, 0);
      cg.applyRegisteredPhases();
      cg.removeAllEstimatorPhases();
    }
    /*} else {
      std::cerr << "Unknown file format given for static call graph" << std::endl;
      return -1;
    */
  }

  if (result.count("cube")) {
    // for dynamic instrumentation
    std::string filePath(result["cube"].as<std::string>());

    std::string fileName = filePath.substr(filePath.find_last_of('/') + 1);
    c.appName = fileName.substr(0, fileName.find_last_of('.'));  // remove .*

    if (stringEndsWith(filePath, ".cubex")) {
      CubeCallgraphBuilder::buildFromCube(filePath, &c, cg);
    } else if (stringEndsWith(filePath, ".dot")) {
      cg = DOTCallgraphBuilder::build(filePath, &c);
    } else {
      std::cerr << "ERROR: Unknown file ending in " << filePath << std::endl;
      exit(-1);
    }

    c.totalRuntime = c.actualRuntime;
    /* This runtime threshold currently unused */
    registerEstimatorPhases(cg, &c, false, runTimeThreshold);
    std::cout << "Registered estimator phases.\n";
  }

  if (result["extrap"].count()) {
    cg.attachExtrapModels();
    cg.printDOT("extrap");

    if (applyModelFilter) {
      std::cout << "Applying model filter" << std::endl;
      cg.registerEstimatorPhase(new pira::ExtrapLocalEstimatorPhaseSingleValueFilter(1.0, true));
    } else {
      std::cout << "Applying model expander" << std::endl;
      cg.registerEstimatorPhase(new RemoveUnrelatedNodesEstimatorPhase(true, false));  // remove unrelated
      cg.registerEstimatorPhase(new pira::ExtrapLocalEstimatorPhaseSingleValueExpander(1.0, true));
    }
  }

  if (cg.hasPassesRegistered()) {
    cg.applyRegisteredPhases();
  }

  return EXIT_SUCCESS;
}
