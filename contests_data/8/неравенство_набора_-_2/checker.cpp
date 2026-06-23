#include "testlib.h"
#include <string>
#include <vector>
#include <sstream>

using namespace std;


int main(int argc, char * argv[]) {
    setName("Check prefixes of files, 0.01 points for each correct answer");
    registerTestlibCmd(argc, argv);

    int right_answers = 0, total_answers = 0;
    while (!ouf.eof()) {
        string participant_answer = ouf.readString();

        if (participant_answer.empty() && ouf.eof()) {
            break;
        }

        string jury_answer = ans.readString();
        if (jury_answer.empty() && ans.eof()) {
            break;
        }
        total_answers++;
        if (trim(participant_answer) == trim(jury_answer)) {
            right_answers++;
        }
    }
    quitp((double) right_answers / 100, "%d answers given, %d right", total_answers, right_answers);
}
