/*   Copyright 2013 Juan Rada-Vilela

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/*
 * main.cpp
 *
 *  Created on: 2/12/2012
 *      Author: jcrada
 */

#include "fl/Headers.h"

#include <typeinfo>
#include <iomanip>
#include <cstdlib>
#include <signal.h>
#include <fstream>
#include <sys/time.h>
#include "fl/LoggerCpp/LoggerCpp.h"

using namespace fl;








/*
void baz(){
    int *x = (int*) - 1; // make a bad pointer
    FL_LOG("%d\n" <<  *x); // causes segfault
}

void bar(){
    baz();
}

void foo(){
    bar();
}
 */



void exportAllExamples(const std::string& from, const std::string& to) {
    std::vector<std::string> examples;
    examples.push_back("/mamdani/AllTerms");
    examples.push_back("/mamdani/SimpleDimmer");
    examples.push_back("/mamdani/matlab/mam21");
    examples.push_back("/mamdani/matlab/mam22");
    examples.push_back("/mamdani/matlab/shower");
    examples.push_back("/mamdani/matlab/tank");
    examples.push_back("/mamdani/matlab/tank2");
    examples.push_back("/mamdani/matlab/tipper");
    examples.push_back("/mamdani/matlab/tipper1");
    examples.push_back("/mamdani/octave/investment_portfolio");
    examples.push_back("/mamdani/octave/mamdani_tip_calculator");
    examples.push_back("/takagi-sugeno/approximation");
    examples.push_back("/takagi-sugeno/SimpleDimmer");
    examples.push_back("/takagi-sugeno/matlab/fpeaks");
    examples.push_back("/takagi-sugeno/matlab/invkine1");
    examples.push_back("/takagi-sugeno/matlab/invkine2");
    examples.push_back("/takagi-sugeno/matlab/juggler");
    examples.push_back("/takagi-sugeno/matlab/membrn1");
    examples.push_back("/takagi-sugeno/matlab/membrn2");
    examples.push_back("/takagi-sugeno/matlab/slbb");
    examples.push_back("/takagi-sugeno/matlab/slcp");
    examples.push_back("/takagi-sugeno/matlab/slcp1");
    examples.push_back("/takagi-sugeno/matlab/slcpp1");
    examples.push_back("/takagi-sugeno/matlab/sltbu_fl");
    examples.push_back("/takagi-sugeno/matlab/sugeno1");
    examples.push_back("/takagi-sugeno/matlab/tanksg");
    examples.push_back("/takagi-sugeno/matlab/tippersg");
    examples.push_back("/takagi-sugeno/octave/cubic_approximator");
    examples.push_back("/takagi-sugeno/octave/heart_disease_risk");
    examples.push_back("/takagi-sugeno/octave/linear_tip_calculator");
    examples.push_back("/takagi-sugeno/octave/sugeno_tip_calculator");
    examples.push_back("/tsukamoto/tsukamoto");

    std::string sourceBase = "/home/jcrada/Development/fl/fuzzylite/examples";
    //    std::string targetBase = "/home/jcrada/Development/fuzzylite/examples/" + to;
    std::string targetBase = "/tmp/fl/";

    Importer* importer;
    if (from == "fll") importer = new FllImporter;
    else if (from == "fis") importer = new FisImporter;
    else if (from == "fcl") importer = new FclImporter;
    else throw fl::Exception("[examples error] unrecognized format <" + from + "> to import", FL_AT);

    Exporter* exporter;
    if (to == "fll") exporter = new FllExporter;
    else if (to == "fld") exporter = new FldExporter(" ", 1024);
    else if (to == "fcl") exporter = new FclExporter;
    else if (to == "fis") exporter = new FisExporter;
    else if (to == "cpp") exporter = new CppExporter;
    else if (to == "java") exporter = new JavaExporter;
    else throw fl::Exception("[examples error] unrecognized format <" + to + "> to export", FL_AT);

    std::vector<std::pair<Exporter*, Importer*> > tests;
    tests.push_back(std::pair<Exporter*, Importer*>(new FllExporter, new FllImporter));
    tests.push_back(std::pair<Exporter*, Importer*>(new FclExporter, new FclImporter));
    tests.push_back(std::pair<Exporter*, Importer*>(new FisExporter, new FisImporter));
    std::ostringstream errors;
    for (std::size_t i = 0; i < examples.size(); ++i) {
        FL_LOG("Processing " << (i + 1) << "/" << examples.size() << ": " << examples.at(i));
        try {
            std::ostringstream ss;
            std::string input = sourceBase + examples.at(i) + "." + from;
            std::ifstream source(input.c_str());
            if (source.is_open()) {
                std::string line;
                while (source.good()) {
                    std::getline(source, line);
                    ss << line << "\n";
                }
                source.close();
            } else throw fl::Exception("[examples error] file not found: " + input, FL_AT);

            Engine* engine = importer->fromString(ss.str());

            for (std::size_t t = 0; t < tests.size(); ++t) {
                std::string out = tests.at(t).first->toString(engine);
                Engine* copy = tests.at(t).second->fromString(out);
                std::string out_copy = tests.at(t).first->toString(copy);

                if (out != out_copy) {
                    errors << "[imex error] different results <"
                            << importer->name() << "," << exporter->name() << "> "
                            "at " + examples.at(t) + "." + from + ":\n";
                }
            }

            std::string output = targetBase + examples.at(i) + "." + to;
            std::ofstream target(output.c_str());
            if (target.is_open()) {
                if (to == "cpp") {
                    target << "#include <fl/Headers.h>\n\n"
                            << "int main(int argc, char** argv){\n"
                            << exporter->toString(engine)
                            << "\n}\n";
                } else if (to == "java") {
                    std::string className = examples.at(i).substr(examples.at(i).find_last_of('/') + 1);
                    target << "import com.fuzzylite.*;\n"
                            << "import com.fuzzylite.defuzzifier.*;\n"
                            << "import com.fuzzylite.factory.*;\n"
                            << "import com.fuzzylite.hedge.*;\n"
                            << "import com.fuzzylite.imex.*;\n"
                            << "import com.fuzzylite.norm.*;\n"
                            << "import com.fuzzylite.norm.s.*;\n"
                            << "import com.fuzzylite.norm.t.*;\n"
                            << "import com.fuzzylite.rule.*;\n"
                            << "import com.fuzzylite.term.*;\n"
                            << "import com.fuzzylite.variable.*;\n\n"
                            << "public class " << Op::makeValidId(className) << "{\n"
                            << "public static void main(String[] args){\n"
                            << exporter->toString(engine)
                            << "\n}\n}\n";
                } else {
                    target << exporter->toString(engine);
                }
                target.close();
            }
            delete engine;
        } catch (fl::Exception& ex) {
            errors << "error at " << examples.at(i) << ":\n" << ex.what() << "\n";
        }
    }
    delete importer;
    delete exporter;
    for (std::size_t i = 0; i < tests.size(); ++i) {
        delete tests.at(i).first;
        delete tests.at(i).second;
    }
    if (errors.str().empty()) {
        FL_LOG("No errors were found exporting files");
    } else {
        FL_LOG("The following errors were encountered while exporting files:" << errors.str());
    }
}

class TestLog : public Log::Output {
	public:
	    explicit TestLog() {
	    };
	    virtual ~TestLog() {

	    }
	    virtual void output(const Log::Channel::Ptr& aChannelPtr, const Log::Log& aLog) const {
	    	(void)aChannelPtr;
	    	std::cout << "test: " << aLog.getStream().str() << std::endl;
	    }
};


int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    fl::fuzzylite::setLogging(true);
    fl::fuzzylite::setDebug(true);

    Log::Config::Vector configList;
#if 0
    Log::Config::addOutput(configList, "OutputConsole");
    Log::Config::addOutput(configList, "OutputFile");
    Log::Config::setOption(configList, "filename",          "log.txt");
    Log::Config::setOption(configList, "filename_old",      "log.old.txt");
    Log::Config::setOption(configList, "max_startup_size",  "0");
    Log::Config::setOption(configList, "max_size",          "10000");
#endif
#ifdef WIN32
    Log::Config::addOutput(configList, "OutputDebug");
#endif

    // Create a Logger object, using a "Main.Example" Channel
    Log::Logger logger22("Main.Example");


    logger22.warning() << "NO logs before configure()";

    try
    {
        // Configure the Log Manager (create the Output objects)
        //Log::Manager::configure(configList);
    	Log::Manager::setCustomLogger(new TestLog());
    }
    catch (std::exception& e)
    {
        std::cerr << e.what();
    }

    // Test outputs of various kind of variables, and some common stream manipulations.
    std::string     str("string");
    unsigned int    ui  = 123;
    double          dbl = -0.023f;
    logger22.debug() << "Variables ; '" << str << "', '" << ui << "', '" << dbl << "'";
    logger22.debug() << "Hexa = " << std::hex << 0x75af0 << " test";
    logger22.debug() << "Deci = " << std::right << std::setfill('0') << std::setw(8) << 76035 << " test";
    logger22.debug() << "sizeof(logger)=" << sizeof(logger22);

    // Test outputs of various severity Level
    logger22.debug()  << "Debug.";
    logger22.info()   << "Info.";
    logger22.notice() << "Notice.";
    logger22.warning()<< "Warning.";
    logger22.error()  << "Error.";
    logger22.critic() << "Critic.";

    // Modify the output Level of the underlying Channel, and test various severity Level again
    logger22.setLevel(Log::Log::eWarning);
    logger22.debug()  << "NO Debug.";     // NO more debug logs
    logger22.info()   << "NO Info.";      // NO more info logs
    logger22.notice() << "NO Notice.";    // NO more notice logs
    logger22.warning()<< "Warning.";
    logger22.error()  << "Error.";
    logger22.critic() << "Critic.";

    // Reset Level of the "Main.example" channel by its name
    Log::Manager::get("Main.Example")->setLevel(Log::Log::eDebug);

    // Create other loggers, sharing the "Main.Example" Channel, and creating a new one
    Log::Logger logger2("Main.Example");
    Log::Logger logger3("Main.Other");
    logger22.debug() << "First logger to the Channel";
    logger2.debug() << "Second logger to the Channel";
    logger3.debug() << "Third logger, other Channel";
    // Modify the Level of the "Main.example" channel by its name
    Log::Manager::get("Main.Example")->setLevel(Log::Log::eInfo);
    logger22.debug() << "first logger inhibited";         // NO more debug logs for this logger
    logger2.debug() << "second logger also disabled";   // NO more debug logs (sharing the same underlying channel)
    logger3.debug() << "third logger still active";
    // Reset the Level of the "Main.example" channel by its name
    Log::Manager::get("Main.Example")->setLevel(Log::Log::eDebug);
    logger22.debug() << "first logger re-activated";
    logger2.debug() << "second logger also re-activated";
    logger3.debug() << "third logger always active";






    std::set_terminate(fl::Exception::terminate);
    std::set_unexpected(fl::Exception::terminate);
    signal(SIGSEGV, fl::Exception::signalHandler);
    signal(SIGABRT, fl::Exception::signalHandler);
    signal(SIGILL, fl::Exception::signalHandler);
    signal(SIGSEGV, fl::Exception::signalHandler);
    signal(SIGFPE, fl::Exception::signalHandler);

#ifdef FL_UNIX
    signal(SIGBUS, fl::Exception::signalHandler);
    signal(SIGPIPE, fl::Exception::signalHandler);
#endif

    try {
//        fuzzylite::setDecimals(3);
//        exportAllExamples("fis", "fll");
//        exportAllExamples("fis", "fcl");
//        exportAllExamples("fis", "fis");
//        exportAllExamples("fis", "cpp");
//        exportAllExamples("fis", "java");
//        fuzzylite::setDecimals(8);
//        exportAllExamples("fis", "fld");
//        return 0;
//        return Console::main(argc, argv);
fl::Engine* engine = new fl::Engine("simple-dimmer");

 fl::InputVariable* ambient = new fl::InputVariable;
 ambient->setName("Ambient");
 ambient->setRange(0.000, 1.000);
 ambient->addTerm(new fl::Triangle("DARK", 0.000, 0.500));
 ambient->addTerm(new fl::Triangle("MEDIUM", 0.250, 0.750));
 ambient->addTerm(new fl::Triangle("BRIGHT", 0.500, 1.000));
 ambient->addTerm(new fl::DateTime("Now", DateTime::TIME_GREATERTHAN));
 engine->addInputVariable(ambient);

 fl::InputVariable* ambient1 = new fl::InputVariable;
 ambient1->setName("Ambient1");
 ambient1->setRange(0.000, 1.000);
 ambient1->addTerm(new fl::Triangle("DARK", 0.000, 0.500));
 ambient1->addTerm(new fl::Triangle("MEDIUM", 0.250, 0.750));
 ambient1->addTerm(new fl::Triangle("BRIGHT", 0.500, 1.000));
 engine->addInputVariable(ambient1);


 fl::OutputVariable* power = new fl::OutputVariable;
 power->setName("Power");
 power->setRange(0.000, 2.000);
 power->setDefaultValue(fl::nan);
 power->addTerm(new fl::Triangle("LOW", 0.000, 1.000));
 power->addTerm(new fl::Triangle("MEDIUM", 0.500, 1.500));
 power->addTerm(new fl::Triangle("HIGH", 1.000, 2.000));
 engine->addOutputVariable(power);


 fl::OutputVariable* power1 = new fl::OutputVariable;
 power1->setName("Power1");
 power1->setRange(0.000, 2.000);
 power1->setDefaultValue(fl::nan);
 power1->addTerm(new fl::Triangle("LOW", 0.000, 1.000));
 power1->addTerm(new fl::Triangle("MEDIUM", 0.500, 1.500));
 power1->addTerm(new fl::Triangle("HIGH", 1.000, 2.000));
 engine->addOutputVariable(power1);

 fl::RuleBlock* ruleblock = new fl::RuleBlock;
 ruleblock->addRule(fl::Rule::parse("if Ambient1 is DARK and Ambient is Now(2:27pm) then Power is very HiGh", engine));
 ruleblock->addRule(fl::Rule::parse("if Ambient is DARK then Power is HIGH", engine));
 ruleblock->addRule(fl::Rule::parse("if Ambient is MEDIUM then Power is MEDIUM", engine));
 ruleblock->addRule(fl::Rule::parse("if Ambient is BRIGHT then in 5m set Power is LOW", engine));
 ruleblock->addRule(fl::Rule::parse("if Ambient is BRIGHT then in 533m set Power is LOW and Power1 is HIGH", engine));
//# ruleblock->addRule(fl::Rule::parse("if Ambient is BRIGHT then in 5m Power is LOW", engine));
 engine->addRuleBlock(ruleblock);

 //No Conjunction or Disjunction is needed
 engine->configure("AlgebraicProduct", "", "AlgebraicProduct", "AlgebraicSum", "Centroid");

 std::string status;
 if (not engine->isReady(&status))
      throw fl::Exception("Engine not ready. "
            "The following errors were encountered:\n" + status, FL_AT);

 struct timeval tv;
 gettimeofday (&tv, NULL);
 ambient->setInputValue(tv.tv_sec);
 ambient1->setInputValue(0.1);
 engine->process();
 FL_LOG("Ambient.input = " << tv.tv_sec << " -> " <<
        "Power.output = " << fl::Op::str(power->defuzzify()) <<
                  " Timer: " << power->getTimer() <<
        " Power1.output = " << fl::Op::str(power1->defuzzify()) <<
        " Timer: " << power1->getTimer());
#if 0
 for (int i = 0; i < 50; ++i){
     fl::scalar light = ambient->getMinimum() + i * (ambient->range() / 50);
     ambient->setInputValue(light);
     engine->process();
     FL_LOG("Ambient.input = " << fl::Op::str(light) << " -> " <<
            "Power.output = " << fl::Op::str(power->defuzzify()) <<
                      " Timer: " << power->getTimer() <<
            " Power1.output = " << fl::Op::str(power1->defuzzify()) <<
            " Timer: " << power1->getTimer());
 }
#endif
 fl::fuzzylite::logger->debug() << engine->toString();

    Exporter* exporter;
    exporter = new JavaExporter;
    fl::fuzzylite::logger->debug() << exporter->toString(engine);
#if 0
    else if (to == "fld") exporter = new FldExporter(" ", 1024);
    else if (to == "fcl") exporter = new FclExporter;
    else if (to == "fis") exporter = new FisExporter;
    else if (to == "cpp") exporter = new CppExporter;
    else if (to == "java") exporter = new JavaExporter;
#endif



    } catch (fl::Exception& e) {
        FL_LOG(e.what());
        FL_LOG(e.btCallStack());
        return EXIT_FAILURE;
    }
}

