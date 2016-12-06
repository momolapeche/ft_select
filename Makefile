# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rmenegau <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2016/02/16 11:16:50 by rmenegau          #+#    #+#              #
#    Updated: 2016/11/17 17:55:13 by rmenegau         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_select
SRC = main.c
OBJ = $(SRC:.c=.o)
LIBFT = ../libft
FLAGS = #-Wall -Wextra -Werror

all: $(NAME)

%.o: %.c
	gcc -o $@ -c $< -I $(LIBFT)/includes $(FLAGS)

$(NAME): $(OBJ)
	make -C $(LIBFT) re
	gcc -o $(NAME) $^ -ltermcap -L$(LIBFT) -lft $(FLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
