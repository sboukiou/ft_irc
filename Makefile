CXX=c++
CPPFLAGS=-Wall -Werror -Wextra -std=c++98

SRCS_DIR=srcs
OBJS_DIR=objs
SRCS=$(SRCS_DIR)/Server.cpp  $(SRCS_DIR)/Client.cpp $(SRCS_DIR)/main.cpp $(SRCS_DIR)/Command.cpp $(SRCS_DIR)/Response.cpp
OBJS=$(OBJS_DIR)/Server.o $(OBJS_DIR)/Client.o $(OBJS_DIR)/main.o $(OBJS_DIR)/Command.o $(OBJS_DIR)/Response.o
RM=rm -rf
NAME=ircserv

all: $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(NAME): $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $(NAME)


clean:
	$(RM) $(OBJS)
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
