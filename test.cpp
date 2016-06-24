#include "vdif_assembler.cpp"
using namespace std;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		cout << "run with:test <network/disk> <port/filelist> " << endl;
	}

	vdif_assembler a(argv[1],argv[2]);
	vdif_processor p;
	a.register_processor(&p);
	a.run();
}

