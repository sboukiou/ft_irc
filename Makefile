CXX=c++
CPPFLAGS=-Wall -Werror -Wextra -std=c++98

SRCS_DIR=srcs
OBJS_DIR=objs
SRCS=$(SRCS_DIR)/main.cpp
OBJS=$(OBJS_DIR)/main.o
RM=rm -rf
NAME=ircserv

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(NAME): $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJS)
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
