CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -MMD -MP
SRCS = ./main.cpp \
		./IOMultiplexing.cpp \
		./utils.cpp \

OBJS = $(SRCS:.cpp=.o)
NAME = ./serv

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

# $(NAME): $(OBJS)
# 		ar r $(NAME) $(OBJS)

test: all
	./serv 8080 8081 8082

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)
	rm -f *.d

re: fclean all

.PHONY: all clean fclean re test