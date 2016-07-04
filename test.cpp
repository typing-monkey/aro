#include "vdif_assembler.cpp"
using namespace std;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "run with:test <network/disk/simulate> <port/filelist> " << endl;
		exit(10);
	}

	vdif_assembler a(argv[1],argv[2]);
	vdif_processor p("test.dat");
	a.register_processor(&p);
	a.run();
}

