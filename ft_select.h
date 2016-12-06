/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_select.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmenegau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/11/21 16:32:00 by rmenegau          #+#    #+#             */
/*   Updated: 2016/12/06 21:17:16 by rmenegau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SELECT_H
# define FT_SELECT_H

struct s_env;

typedef void	(*t_func)(struct s_env const, int, int);

typedef struct	s_coor
{
	int	x;
	int	y;
}				t_coor;

typedef struct	s_env
{
	struct winsize	w;
	char			**arg;
	int				*arg_len;
	int				*arg_select;
	int				*arg_x;
	int				nb_arg;
	int				arg_i;
	t_coor			cursor_pos;
	t_func			print[2];
}				t_env;

#endif
