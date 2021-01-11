#include"Grammar.h"

void Grammar::Generate()
{
	string res = Result;					// init resz
	int	num = Generation.size();			// num of replace rules

	for (int i = 0; i < Level; i++) {
		// process i Level replacement

		for (unsigned int len = 0; len < res.length(); len++) {
			// each time we process a char in res and check replace

			for (int nnum = 0; nnum < num; nnum++) {
				// check for each replace tule

				if (res[len] == Generation[nnum].key) {
					// if this char can be replaced

					res.replace(len, 1, Generation[nnum].t);
					len += Generation[nnum].t.length() - 1;	// we need to change current length to avoid replace sub-level char
				}
				else {
					// if this char can't be replaced

					continue;
				}
			}
		}
	}

	Result = res;
}

void Grammar::print()
{
	cout << "Axiom: " << Axiom << endl;
	cout << "Level: " << Level << endl;
	cout << "Generations: " << endl;
	for (unsigned int i = 0; i < Generation.size(); i++) {
		cout << "\t";
		Generation[i].print();
		cout << endl;
	}
	cout << "Result: " << Result << endl;
}

void Grammar::defaultinit()
{
	Axiom = "F";
	Level = 6;
	Result = Axiom;

	Tuple test;
	test.key = 'F'; test.t = "F[+F][-F][^F][&F]";

	vector<Tuple> newv;
	newv.push_back(test);

	Generation = newv;
	this->Generate();

	cout << "Default initialize..." << endl;
	this->print();
}