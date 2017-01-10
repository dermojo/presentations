/**
 * @file		main.cc
 * @author	daniel
 * @brief	Hauptprogramm
 */

#include "doors.h"

#include <thread>
#include <boost/algorithm/string.hpp>

int main()
{
   Doors doors;
   std::thread thread([&]() { doors.run(); });

   while (true)
   {
      std::string line;
      std::cout << "> ";
      std::cout.flush();
      std::getline(std::cin, line);
      if (std::cin.eof()) break;

      boost::trim(line);
      if (line == "quit" || line == "exit") break;

      if (boost::starts_with(line, "m"))
         doors.motionDetected();
      else if (boost::starts_with(line, "l"))
         doors.setLocked(true);
      else if (boost::starts_with(line, "u"))
         doors.setLocked(false);
      else if (!line.empty())
      {
         std::cout << "command: quit, exit, mo[tion], l[ock], u[nlock]\n";
      }
   }

   doors.stop();
   thread.join();

   return 0;
}
