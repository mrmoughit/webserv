SRC = request/request.cpp \
	  request/request_implementation.cpp \
	  request/tools.cpp \
	  c_tools/tools.cpp \
	  response/response.cpp\
	  client/client.cpp \

OBJ = $(SRC:.cpp=.o)
NAME = webserv
HEDER = request/request.hpp
CFLAGS =  -Wall -Wextra -Werror -std=c++98

all : $(NAME)

%.o : %.cpp ${HEDER} Makefile
	c++ ${CFLAGS} -c $< -o $@

$(NAME) : $(OBJ) ${HEDER}
	 c++ ${CFLAGS} $(OBJ) -o $@

clean :
	rm -rf $(OBJ)

fclean : clean
	rm -rf $(NAME)

re : fclean all

.PHONY : clean