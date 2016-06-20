#include "vdif_assembler.cpp"
using namespace std;



int main() {
	vdif_assembler a;
	vdif_processor p;
	a.register_processor(&p);
	a.run();
}

