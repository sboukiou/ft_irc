#ifndef MACROS_HPP
# define MACROS_HPP

# define RED     "\x1b[31m"
# define GREEN   "\x1b[32m"
# define YEL  "\x1b[33m"
# define BLUE    "\x1b[34m"
# define RESET   "\x1b[0m"

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>
#include <sstream>

# define LOG(X) std::cout << BLUE << X << RESET << std::endl;
# define WARN(X) std::cout << YEL << X << RESET << std::endl;
# define ERR(X) std::cout << RED << X << RESET << std::endl;

#endif /* MACROS_HPP */
