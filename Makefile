NAME = webserv
CPPC = c++
FLGS = -std=c++98 #-Wall -Wextra -Werror -g -fsanitize=address
INCS = config/ConfigParser.hpp \
		config/ConfigFile.hpp \
		config/LocationConfig.hpp \
		config/ServerConfig.hpp \
		server/Server.hpp \
		# httpRequest/httpRequest.hpp
SRCS = main.cpp \
		config/ConfigParser.cpp \
		config/ConfigFile.cpp \
		server/Server.cpp \
		# httpRequest/httpRequest.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

%.o: %.cpp $(INCS)
	$(CPPC) $(FLGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CPPC) $(FLGS) $(OBJS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all