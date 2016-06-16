#import <thread>
#import "vdif_assembler.cpp"

namespace vdif_assember{
void vdif_assembler::start_async(){
	std::thread (this.run).detach();
}


}