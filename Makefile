SRC = main.cpp server/Server.cpp config/ConfigFile.cpp config/LocationConfig.cpp config/ServerConfig.cpp

OBJ = ${SRC:.cpp=.o}

FLAGS = 

NAME = webserv

all: ${NAME}

${NAME}: ${OBJ}
	c++ ${FLAGS} ${OBJ} -o ${NAME}

%.o: %.cpp
	c++ ${FLAGS} -c $< -o $@

clean:
	rm -rf ${OBJ}

fclean: clean
	rm -rf ${NAME}

re: fclean all