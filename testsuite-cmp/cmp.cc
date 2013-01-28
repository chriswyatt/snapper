
#include <algorithm>
#include <iostream>
#include <fstream>

#include "snapper/Log.h"
#include "snapper/AppUtil.h"
#include "snapper/File.h"
#include "snapper/Filesystem.h"
#include "snapper/SnapperTmpl.h"


using namespace std;
using namespace snapper;


bool
cmp(const string& fstype, const string& subvolume, unsigned int num1, unsigned int num2)
{
    Filesystem* filesystem = Filesystem::create(fstype, subvolume);

    SDir dir1 = filesystem->openSnapshotDir(num1);
    SDir dir2 = filesystem->openSnapshotDir(num2);

    vector<string> result1;

    {
	StopWatch sw1;

	cmpdirs_cb_t cb1 = [&result1](const string& name, unsigned int status) {
	    result1.push_back(statusToString(status) + " " + name);
	};

	filesystem->cmpDirs(dir1, dir2, cb1);

	y2mil("stopwatch1 " << sw1);

	sort(result1.begin(), result1.end());
    }

    vector<string> result2;

    {
	StopWatch sw2;

	cmpdirs_cb_t cb2 = [&result2](const string& name, unsigned int status) {
	    result2.push_back(statusToString(status) + " " + name);
	};

	snapper::cmpDirs(dir1, dir2, cb2);

	y2mil("stopwatch2 " << sw2);

	sort(result2.begin(), result2.end());
    }

    if (result1 == result2)
    {
	cout << fstype << " " << subvolume << " " << num1 << " " << num2 << " results match" << endl;

	return true;
    }
    else
    {
	cout << fstype << " " << subvolume << " " << num1 << " " << num2 << " results don't match" << endl;

	std::ofstream fout1("/tmp/result1-" + decString(num1) + "-" + decString(num2));
	for (vector<string>::const_iterator it = result1.begin(); it != result1.end(); ++it)
	    fout1 << *it << endl;

	std::ofstream fout2("/tmp/result2-" + decString(num1) + "-" + decString(num2));
	for (vector<string>::const_iterator it = result2.begin(); it != result2.end(); ++it)
	    fout2 << *it << endl;

	return false;
    }
}


int
main(int argc, char** argv)
{
    if (argc != 5)
    {
	cerr << "usage: fstype subvolume num1 num2" << endl;
	exit(EXIT_FAILURE);
    }

    string fstype = argv[1];
    string subvolume = argv[2];
    unsigned int num1 = atoi(argv[3]);
    unsigned int num2 = atoi(argv[4]);

    exit(cmp(fstype, subvolume, num1, num2) ? EXIT_SUCCESS : EXIT_FAILURE);
}
