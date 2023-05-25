CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD -MP -pedantic -fsanitize=address

INCLUDES = -I ./includes

SRCDIR = srcs
OBJDIR = objs

SRCS = $(shell find $(SRCDIR) -type f -name "*.cpp")
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

NAME = webserv

$(NAME): $(OBJDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

$(OBJDIR):
	mkdir $(shell find $(SRCDIR) -type d | sed 's/^$(SRCDIR)/$(OBJDIR)/g')

all: $(NAME)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<


test: all
	./webserv config/default.conf

cgi: all
	./webserv config/cgi.conf ; rm ./contents/cgi_files/*.txt
	

.PHONY: all clean fclean re test cgi