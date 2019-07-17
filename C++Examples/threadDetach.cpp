#include <iostream>       // std::cout
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <unistd.h> 

void thread(int n) 
{
  std::this_thread::sleep_for (std::chrono::seconds(n));
  std::cout << "pause of " << n << " seconds ended\n";
}
 
int main() 
{
	std::cout << "Start main\n";
	std::thread t(thread,1);
	std::thread t2(thread,3);
	std::thread t3(thread,8);
	sleep(5);

	t.detach();
	t2.detach();
	t3.detach();
	return 0;
}
