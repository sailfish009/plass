#include "Parameters.h"
#include "Util.h"
#include "FileUtil.h"
#include "Debug.h"
#include "UniprotKB.h"
#include "Log.h"

#include <fstream>

int kbtotsv(int argn, const char **argv) {
    std::string usage;
    usage.append("Turns an UniprotKB file into separate TSV tables.\n");
    usage.append("USAGE: <uniprotKB> <kbCSV> <accessionCSV>\n");
    usage.append("\nDesigned and implemented by Milot Mirdita <milot@mirdita.de>.\n");

    Parameters par;
    par.parseParameters(argn, argv, usage, par.onlyverbosity, 1);

    std::ifstream kbIn(par.db1);
    if (kbIn.fail()) {
        Debug(Debug::ERROR) << "File " << par.db1 << " not found!\n";
        EXIT(EXIT_FAILURE);
    }

    std::ofstream tsvOut(par.db2);
    if (tsvOut.fail()) {
        Debug(Debug::ERROR) << "Could not open " << par.db2 << " for writing!\n";
        EXIT(EXIT_FAILURE);
    }

    std::ofstream accessionOut(par.db3);
    if (accessionOut.fail()) {
        Debug(Debug::ERROR) << "Could not open " << par.db3 << " for writing!\n";
        EXIT(EXIT_FAILURE);
    }

    UniprotKB kb;
    std::string line;
    size_t i = 0;
    while (std::getline(kbIn, line)) {
        if (line.length() < 2) {
            Debug(Debug::WARNING) << "Invalid line" << "\n";
            continue;
        }

        if(kb.readLine(line.c_str())) {
            Log::printProgress(i);
            for (size_t i = 0; i < kb.getColumnCount() - 2; ++i) {
                tsvOut << Util::csvEscape(kb.getColumn(i)) << "\t";
            }
            tsvOut << Util::csvEscape(kb.getColumn(kb.getColumnCount() - 1)) << "\n";

            std::string accessions = kb.getColumn(UniprotKB::COL_KB_AC);
            std::string identifier = kb.getColumn(UniprotKB::COL_KB_ID);

            std::vector<std::string> acs = Util::split(accessions, ";");

            for(std::vector<std::string>::const_iterator it = acs.begin(); it != acs.end(); ++it) {
                std::string ac = *it;
                accessionOut << ac << "\t" << identifier << "\n";
            }

            i++;
        }
    }

    accessionOut.close();
    tsvOut.close();
    kbIn.close();


    return EXIT_SUCCESS;
}
