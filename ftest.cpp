 






#include "platform_compat.hpp"
#include "platform_compat.hpp"

int main() {
	const char *prima = "Prima",
		 *dopo  = "Dopo";
	int handle;

	if ((handle = _creat("test.lsk",0x20)) >= 0) {
		_write(handle,dopo,5);
		_write(handle,dopo,4);
		lseek(handle,0,SEEK_SET);
		_write(handle,prima,5);
		lseek(handle,0,SEEK_END);
		_write(handle,dopo,4);
		_close(handle);
	}
	return 0;
}