/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmenegau <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/11/17 17:39:48 by rmenegau          #+#    #+#             */
/*   Updated: 2016/12/06 21:26:08 by rmenegau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <term.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "libft.h"
#include "ft_select.h"

int		my_tput(int c)
{
	return (write(1, &c, 1));
}

int	show(void)
{
	char	buf[4];
	int		mem;

	mem = read(0, buf, 4);
	if (mem > 1 && *((short*)buf) == 0x1b5b)
		return (1);
	return (0);
}

char	**get_arg(int ac, char **av)
{
	char	**ret;

	ret = (char **)malloc(sizeof(char *) * ac);
	ft_memcpy(ret, &av[1], ac * sizeof(char *));
	return (ret);
}

int		get_nb_spaces(char **arg, int ac)
{
	int	i;
	int	len;
	int ret;

	i = 0;
	ret = 0;
	len = 0;
	while (i < ac)
	{
		if ((len = ft_strlen(arg[i])) > ret)
			ret = len;
		i++;
	}
	return (ret + 1);
}

void	print_arg(t_env const e, int y, int i)
{
	if (e.arg_i == i)
		tputs(tgetstr("us", NULL), 1, my_tput);
	tputs(tgoto(tgetstr("cm", NULL), e.arg_x[i], y), 1, my_tput);
	write(1, e.arg[i], e.arg_x[i] + e.arg_len[i] < e.w.ws_col ? e.arg_len[i] : e.w.ws_col - e.arg_x[i]);
	if (e.arg_i == i)
		tputs(tgetstr("ue", NULL), 1, my_tput);
}

void	print_arg_select(t_env const e, int y, int i)
{
	write(1, "\033[7m", 4);
	tputs(tgoto(tgetstr("cm", NULL), e.arg_x[i], y), 1, my_tput);
	if (e.arg_i == i)
		tputs(tgetstr("us", NULL), 1, my_tput);
	write(1, e.arg[i], e.arg_x[i] + e.arg_len[i] < e.w.ws_col ? e.arg_len[i] : e.w.ws_col - e.arg_x[i]);
	if (e.arg_i == i)
		tputs(tgetstr("ue", NULL), 1, my_tput);
	write(1, "\033[0m", 4);
}

t_env	get_env(char **arg, int nb)
{
	t_env	e;
	int		i;

	ioctl(0, TIOCGWINSZ, &e.w);
	e.arg = arg;
	e.arg_len = (int *)malloc(sizeof(int) * nb);
	e.arg_select = (int *)ft_memalloc(sizeof(int) * nb);
	e.arg_x = (int *)ft_memalloc(sizeof(int) * nb);
	e.nb_arg = nb;
	e.print[0] = print_arg;
	e.print[1] = print_arg_select;
	i = 0;
	while (i < nb)
	{
		e.arg_len[i] = ft_strlen(e.arg[i]);
		i++;
	}
	e.arg_i = 0;
	e.cursor_pos.x = 0;
	e.cursor_pos.y = 0;
	return (e);
}

void	print_list(t_env const e, int *arg_x)
{
	int		tmp;
	t_coor	pos;
	int		i;

	tputs(tgetstr("cl", NULL), 1, my_tput);
	pos.x = 0;
	i = 0;
	while (pos.x < e.w.ws_col && i < e.nb_arg)
	{
		pos.y = 0;
		tmp = 0;
		while (pos.y < e.w.ws_row && i < e.nb_arg)
		{
			if (e.arg_len[i] > tmp)
				tmp = e.arg_len[i];
			arg_x[i] = pos.x;
			e.print[e.arg_select[i]](e, pos.y++, i);
			i++;
		}
		pos.x += tmp + 1;
	}
}

void	move_ver(t_env *e, int dir)
{
	int	tmp;

	tmp = e->arg_i;
	if (e->nb_arg == 1)
		return ;
	if (!e->arg_i && !dir)
		e->arg_i = e->nb_arg - 1;
	else if (e->arg_i == e->nb_arg - 1 && dir)
		e->arg_i = 0;
	else
		e->arg_i += dir ? 1 : -1;
	e->print[e->arg_select[tmp]](*e, tmp % e->w.ws_row, tmp);
	e->print[e->arg_select[e->arg_i]](*e, e->arg_i % e->w.ws_row, e->arg_i);
}

void	move_hor(t_env *e, int dir)
{
	int	tmp;

	if (e->nb_arg <= e->w.ws_row)
		return ;
	tmp = e->arg_i;
	if (e->arg_i >= e->w.ws_row && !dir)
		e->arg_i -= e->w.ws_row;
	else if (e->arg_i + e->w.ws_row < e->nb_arg && dir)
		e->arg_i += e->w.ws_row;
	e->print[e->arg_select[tmp]](*e, tmp % e->w.ws_row, tmp);
	e->print[e->arg_select[e->arg_i]](*e, e->arg_i % e->w.ws_row, e->arg_i);
}

void	select_arg(t_env *e)
{
	int	tmp;

	tmp = e->arg_i;
	e->arg_select[e->arg_i++] ^= 1;
	if (e->arg_i == e->nb_arg)
		e->arg_i = 0;
	e->print[e->arg_select[tmp]](*e, tmp % e->w.ws_row, tmp);
	e->print[e->arg_select[e->arg_i]](*e, e->arg_i % e->w.ws_row, e->arg_i);
}

int	read_input(t_env *e)
{
	static char	buf[3];
	int			mem;

	mem = read(0, buf, 3);
	if (mem == 3)
	{
		if (buf[2] == 'A' || buf[2] == 'B')
			move_ver(e, buf[2] == 'B');
		if (buf[2] == 'C' || buf[2] == 'D')
			move_hor(e, buf[2] == 'C');
		return (1);
	}
	if (mem == 1)
	{
		if (buf[0] == ' ')
		{
			select_arg(e);
			return (1);
		}
	}
	return (0);
}

int	main(int ac, char **av)
{
	char			*term_name;
	struct termios	term;
	t_env			e;

	term_name = getenv("TERM");
	tgetent(NULL, term_name);
	tcgetattr(0, &term);
	term.c_lflag &= ~(ICANON);
	term.c_lflag &= ~(ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;
	tcsetattr(0, TCSADRAIN, &term);
	tputs(tgetstr("vi", NULL), 1, my_tput);

	e = get_env(&av[1], ac - 1);
	print_list(e, e.arg_x);
	int	mem = 1;
	while (mem)
		mem = read_input(&e);
	tputs(tgetstr("ve", NULL), 1, my_tput);
	term.c_lflag |= ICANON;
	term.c_lflag |= ECHO;
	tcsetattr(0, TCSADRAIN, &term);
	return (0);
}
