NAME			= webserv

SRCS_LIST	= \
						main.cpp \
						Utils/get_next_line.cpp \
						Utils/utils.cpp \
						Utils/logger.cpp \
						Utils/page_errors.cpp \
						Utils/file.cpp \
						Config/config.cpp \
						Config/parse_config_server.cpp \
						Config/parse_options.cpp \
						HTTP/Request.cpp \
						HTTP/Response.cpp \
						HTTP/SimpleServer.cpp \
						HTTP/SimpleSocket.cpp \
						HTTP/Cgi.cpp			


SRCS_FOLDER		= srcs

SRCS				= $(addprefix ${SRCS_FOLDER}/, ${SRCS_LIST})

OBJS				= ${SRCS:.cpp=.o}

INCLUDES		= -I includes/Config -I includes/Utils -I includes/HTTP

CXX					= clang++
CXXFLAGS 				= -std=c++98 -pedantic-errors -pthread -fsanitize=address -g3
RM					= rm -f

all:				$(NAME)

$(NAME):		$(OBJS)
						$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

%.o: %.cpp
						${CXX} ${CXXFLAGS} $(INCLUDES) -o $@ -c $<

clean:
						${RM} ${OBJS}

fclean:			clean
						${RM} ${NAME}

re:					fclean all

.PHONY: 		all fclean clean re